#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
#include <linux/poll.h>

#define CHDEV_NAME "poll_chdev_demo"
#define CHDEV_MAX 3
#define KFIFO_MAX 8
typedef struct chdev {
	char dev_name[64];
} chdev_t;
typedef struct chdev_private_data {
	chdev_t *chdev;
	char name[64];
	wait_queue_head_t read_queue;
	wait_queue_head_t write_queue;
	struct kfifo fifo;
} chdev_data_t;
static chdev_t *chdevs[CHDEV_MAX];

static struct cdev *chdev;
static dev_t devt = 0;

static int chdev_open(struct inode *inode, struct file *f){
	int ret;
	unsigned int minor = iminor(inode);
       	chdev_data_t *data;
	chdev_t *dev = chdevs[minor];

	data = kmalloc(sizeof(chdev_data_t), GFP_KERNEL);
	if(!data)
		return -ENOMEM;

	sprintf(data->name, "private_data_%d", minor);

	ret = kfifo_alloc(&data->fifo, KFIFO_MAX, GFP_KERNEL);
	if(ret){
		kfree(data);
		return -ENOMEM;
	}

	init_waitqueue_head(&data->read_queue);
	init_waitqueue_head(&data->write_queue);

	data->chdev = dev;
	f->private_data = data;

	printk("open %s with minor %u\n", CHDEV_NAME, minor);
	return 0;
} 

static int chdev_release(struct inode *inode, struct file *f){ 
	chdev_data_t *pdata = f->private_data;
	chdev_t *chdev = pdata->chdev;

	kfifo_free(&pdata->fifo);
	kfree(pdata);
	printk("%s is closed\n", chdev->dev_name);

	return 0;
} 

static ssize_t chdev_read(struct file *f, char __user *data, size_t count, loff_t *offset){
	int actual_read;
	int ret;
	chdev_data_t *pdata = f->private_data;

	if(kfifo_is_empty(&pdata->fifo)){
		if(f->f_flags & O_NONBLOCK)
			return -EAGAIN;

		// blocking
		printk("%s: pid: %d going to sleep, private_data: %s(read queue addr: %p)\n", __func__, current->pid, pdata->name, &pdata->read_queue);
		ret = wait_event_interruptible(pdata->read_queue, !kfifo_is_empty(&pdata->fifo));
		if(ret)
			return ret;
	}

	ret = kfifo_to_user(&pdata->fifo, data, count, &actual_read);
	if(ret)
		return -EIO;

	if(!kfifo_is_full(&pdata->fifo))
		wake_up_interruptible(&pdata->write_queue);

	printk("%s enter and read %d bytes, offset: %lld\n", __func__, actual_read, *offset);
	return actual_read;
} 

static ssize_t chdev_write(struct file *f, const char __user *data, size_t count, loff_t *offset){
	int actual_write;
	int ret;
	chdev_data_t *pdata = f->private_data;

	if(kfifo_is_full(&pdata->fifo)){
		if(f->f_flags & O_NONBLOCK)
			return -EAGAIN;

		// blocking
		printk("%s: pid: %d going to sleep, private_data: %s(addr: %p)\n", __func__, current->pid, pdata->name, &pdata);
		ret = wait_event_interruptible(pdata->write_queue, !kfifo_is_full(&pdata->fifo));
		if(ret)
			return ret;
	}

	ret = kfifo_from_user(&pdata->fifo, data, count, &actual_write);
	if(ret)
		return -EIO; 
	if(!kfifo_is_empty(&pdata->fifo)){
		wake_up_interruptible(&pdata->read_queue);
		printk("wait up read queue, private data : %s(read queue addr: %p)\n", pdata->name, &pdata->read_queue);
	}

	printk("%s enter and write %d bytes, offset: %lld\n", __func__, actual_write, *offset);
	return actual_write;
} 

static __poll_t chdev_poll(struct file *f, struct poll_table_struct *wait){
	__poll_t mask = 0;
	chdev_data_t *pdata = f->private_data;

	printk("%s entered\n", __func__);

	poll_wait(f, &pdata->read_queue, wait);
	poll_wait(f, &pdata->write_queue, wait);

	if(!kfifo_is_empty(&pdata->fifo))
		mask |= POLLIN | POLLRDNORM;
	if(!kfifo_is_full(&pdata->fifo))
		mask |= POLLOUT | POLLWRNORM;

	return mask;
}

static struct file_operations chdev_fops = {
	.owner = THIS_MODULE,
	.open = chdev_open,
	.release = chdev_release,
	.read = chdev_read,
	.write = chdev_write,
	.poll = chdev_poll
};

static int __init chdev_init(void){
	int ret;
	int i;
	chdev_t *device;

	ret = alloc_chrdev_region(&devt, 0, CHDEV_MAX, CHDEV_NAME);
	if(ret){
		printk("register_chrdev_region failed\n");
		goto clean;
	}

	chdev = cdev_alloc();
	if(!chdev){
		printk("cdev_alloc failed\n");
		goto clean;
	}

	cdev_init(chdev, &chdev_fops);

	ret = cdev_add(chdev, devt, CHDEV_MAX);
	if(ret){
		printk("cdev_add failed\n");
		goto cdev_add_fail;
	}

	for(i = 0; i < CHDEV_MAX; ++i){
		device = kmalloc(sizeof(chdev_t), GFP_KERNEL);
		if(!device){
			ret = -ENOMEM;
			goto free_device;
		}

		sprintf(device->dev_name, "%s%d", CHDEV_NAME, i);
		chdevs[i] = device;
	}

	printk("Successfully registered char device: %s\n", CHDEV_NAME);
	return 0;

free_device:
	for(i = 0; i < CHDEV_MAX; ++i)
		if(chdevs[i])
			kfree(chdevs[i]);

cdev_add_fail:
	cdev_del(chdev);

clean:
	unregister_chrdev_region(devt, CHDEV_MAX);
	return ret;
}

static void __exit chdev_exit(void){
	int i;

	printk("%s has been removed\n", CHDEV_NAME);
	for(i = 0; i < CHDEV_MAX; ++i)
		if(chdevs[i])
			kfree(chdevs[i]);

	cdev_del(chdev);
	unregister_chrdev_region(devt, CHDEV_MAX);
}

module_init(chdev_init);
module_exit(chdev_exit);

MODULE_AUTHOR("Ming");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is a simple I/O multiplexer(support poll syscall) character device");


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

static const char *chdev_name = "chdev_demo";
static dev_t devt;
static struct cdev *chdev;
static struct class *cls = NULL;
static struct device *dev = NULL;

static int chdev_open(struct inode *inode, struct file *f){
	int major = MAJOR(inode->i_rdev);
	int minor = MINOR(inode->i_rdev);

	printk("%s: major: %d, minor: %d\n", chdev_name, major, minor);

	return 0;
} 

static int chdev_release(struct inode *inode, struct file *f){
	printk("%s is closed\n", chdev_name);
	return 0;
} 

static ssize_t chdev_read(struct file *f, char __user *data, size_t count, loff_t *offset){
	printk("%s enter\n", __func__);
	return 0;
} 

static ssize_t chdev_write(struct file *f, const char __user *data, size_t count, loff_t *offset){
	printk("%s enter\n", __func__);
	return 0;
} 

static struct file_operations chdev_fops = {
	.owner = THIS_MODULE,
	.open = chdev_open,
	.release = chdev_release,
	.read = chdev_read,
	.write = chdev_write
};

static int __init chdev_init(void){
	// dynamically find a major number
	int ret;

	ret = alloc_chrdev_region(&devt, 0, 1, chdev_name);
	if(ret){
		printk("alloc_chrdev_region failed\n");
		return 0;
	}

	chdev = cdev_alloc();
	if(!chdev){
		printk("cdev_alloc failed\n");
		goto clean;
	}

	cdev_init(chdev, &chdev_fops);

	ret = cdev_add(chdev, devt, 1);
	if(ret){
		printk("cdev_add failed\n");
		goto fail;
	}

	// class for device_create
	cls = class_create(THIS_MODULE, chdev_name);

	// create a device file on /dev directory which can be accessed by user program
	dev = device_create(cls, NULL, devt, NULL, chdev_name);
	if(IS_ERR(dev)){
		printk("device_create failed\n");
		ret = -PTR_ERR(dev);
		goto fail;
	}

	printk("%s dev has been created\n", chdev_name);
	return 0;

fail:
	cdev_del(chdev);
clean:
	unregister_chrdev_region(devt, 1);
	return ret;
}

static void __exit chdev_exit(void){
	printk("%s has been removed\n", chdev_name);
	cdev_del(chdev);
	unregister_chrdev_region(devt, 1);
	device_destroy(cls, devt);
	class_destroy(cls);
}

module_init(chdev_init);
module_exit(chdev_exit);

MODULE_AUTHOR("Ming");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is a simple character device");


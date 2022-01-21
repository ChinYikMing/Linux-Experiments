#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

#define MISCDEV_NAME "miscdev_demo"
#define MISCDEV_BUFSIZE 8
static char miscdev_buf[MISCDEV_BUFSIZE];
static struct device *dev;

static int miscdev_open(struct inode *inode, struct file *f){
	int major = MAJOR(inode->i_rdev);
	int minor = MINOR(inode->i_rdev);

	printk("%s: major: %d, minor: %d\n", MISCDEV_NAME, major, minor);

	return 0;
} 

static int miscdev_release(struct inode *inode, struct file *f){
	printk("%s is closed\n", MISCDEV_NAME);
	return 0;
} 
static ssize_t miscdev_read(struct file *f, char __user *data, size_t count, loff_t *offset){
	size_t need_read;
	int actual_read;
	int ret;

	need_read = MISCDEV_BUFSIZE - *offset;
	if(need_read == 0)
		dev_warn(dev, "no data to read\n");
	else if(count > need_read)
		count = need_read;

	ret = copy_to_user(data, miscdev_buf + *offset, count);
	if(ret == need_read)
		return -EFAULT;

	// update offset
	actual_read = count - ret;
	if((*offset + actual_read) == MISCDEV_BUFSIZE)
		*offset = 0;
	else
		*offset += actual_read;

	printk("%s enter and read %d bytes\n", __func__, actual_read);
	return actual_read;
} 

static ssize_t miscdev_write(struct file *f, const char __user *data, size_t count, loff_t *offset){
	int capacity_free;
	int ret;
	int actual_write;

	capacity_free = MISCDEV_BUFSIZE - *offset;
	if(count > capacity_free){
		dev_warn(dev, "data is truncated\n");
		count = capacity_free;
	}

	ret = copy_from_user(miscdev_buf + *offset, data, count);
	if(ret == count)
		return -EFAULT;

	// update offset
	actual_write = count - ret;
	if((*offset + actual_write) == MISCDEV_BUFSIZE)
		*offset = 0;
	else
		*offset += actual_write;

	printk("%s enter and write %d bytes\n", __func__, actual_write);
	return actual_write;
} 

static struct file_operations miscdev_fops = {
	.owner = THIS_MODULE,
	.open = miscdev_open,
	.release = miscdev_release,
	.read = miscdev_read,
	.write = miscdev_write
};
static struct miscdevice miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = MISCDEV_NAME,
	.fops = &miscdev_fops
};

static int __init miscdev_init(void){
	// dynamically find a major number
	int ret;

	ret = misc_register(&miscdev);
	if(ret){
		printk("misc_register failed\n");
		return ret;
	}

	dev = miscdev.this_device;

	printk("%s dev has been created\n", MISCDEV_NAME);
	return 0;
}

static void __exit miscdev_exit(void){
	printk("%s has been removed\n", MISCDEV_NAME);
	misc_deregister(&miscdev);
}

module_init(miscdev_init);
module_exit(miscdev_exit);

MODULE_AUTHOR("Ming");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is a simple misc device");


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
#include <linux/miscdevice.h>

#define MISCDEV_NAME "miscdev_demo"
DEFINE_KFIFO(myfifo, char, 8);
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
	int actual_read;
	int ret;

	ret = kfifo_to_user(&myfifo, data, count, &actual_read);
	if(ret)
		return -EIO;

	printk("%s enter and read %d bytes, offset: %lld\n", __func__, actual_read, *offset);
	return actual_read;
} 

static ssize_t miscdev_write(struct file *f, const char __user *data, size_t count, loff_t *offset){
	int actual_write;
	int ret;

	ret = kfifo_from_user(&myfifo, data, count, &actual_write);
	if(ret)
		return -EIO;

	printk("%s enter and write %d bytes, offset: %lld\n", __func__, actual_write, *offset);
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


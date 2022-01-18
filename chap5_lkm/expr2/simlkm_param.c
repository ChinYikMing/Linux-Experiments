#include <linux/init.h>
#include <linux/module.h>

static int x = 1;
module_param(x, int, 0644);
MODULE_PARM_DESC(x, "dummy variable");

static int __init my_init(void){
	printk("Hello\n");
	printk("x: %d\n", x);
	return 0;
}

static void __exit my_exit(void){
	printk("Goodbye\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ming");
MODULE_DESCRIPTION("This is a simple linux kernel module");

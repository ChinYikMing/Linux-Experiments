#include <linux/init.h>
#include <linux/module.h>

extern void foo(void);

static int __init my_init(void){
	printk("Hello\n");
	foo();
	return 0;
}

static void __exit my_exit(void){
	printk("Goodbye\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ming");
MODULE_DESCRIPTION("This is a caller module which call a function from callee module");


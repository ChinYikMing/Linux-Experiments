#include <linux/init.h>
#include <linux/module.h>

void foo(void){
	printk("Hi Foo\n");
}
EXPORT_SYMBOL_GPL(foo);

static int __init my_init(void){
	printk("Hello\n");
	return 0;
}

static void __exit my_exit(void){
	printk("Goodbye\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ming");
MODULE_DESCRIPTION("This is a callee module which exported foo function");

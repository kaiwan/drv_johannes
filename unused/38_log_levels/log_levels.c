#include <linux/module.h>
#include <linux/init.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A hello world LKM");

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void) {
	printk(KERN_DEBUG "This is a debug msg\n");
	printk(KERN_INFO "Info: %d\n", 42);
	printk(KERN_ERR "ERROR!!!\n");
	printk(KERN_ALERT "ALERT!!!\n");
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void) {
	pr_info("Another info\n");
	pr_crit("Critical info: %d\n", 123);
	pr_err("Second Error\n");
}

module_init(my_init);
module_exit(my_exit);



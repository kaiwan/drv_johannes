#include <linux/module.h>
#include <linux/init.h>
#include <linux/completion.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple test for completions");

/** variable for timer */
static struct timer_list my_timer;
static struct completion comp;

void timer_callback(struct timer_list * data) {
	printk("mycompletion - timer expired\n");
	complete(&comp);
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	int status;

	/* Initialize timer */
	timer_setup(&my_timer, timer_callback, 0);
	/* Initialize completion */
	init_completion(&comp);

	printk("mycompletion - Start the timer the first time. Time: 40ms\n");
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(40));
	status = wait_for_completion_timeout(&comp, msecs_to_jiffies(100));
	if(!status) 
		printk("mycompletion - Completion timed out!\n");

	reinit_completion(&comp);
	printk("mycompletion - Start the timer the second time. Time: 400ms\n");
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(400));
	status = wait_for_completion_timeout(&comp, msecs_to_jiffies(100));
	if(!status) 
		printk("mycompletion - Completion timed out!\n");

	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	del_timer(&my_timer);
}

module_init(ModuleInit);
module_exit(ModuleExit);



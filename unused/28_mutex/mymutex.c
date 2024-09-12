#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/mutex.h>

/* Global variables for the threads */
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;
static struct mutex lock;

/**
 * @brief Function which will be executed by the threads
 *
 * @param thread_nr	Pointer to number of the thread
 */
int thread_function(void * thread_nr) {
	int delay[] = { 0, 1000, 500};
	int t_nr = *(int *) thread_nr;

	/* Working loop */
	printk("mymutex - Thread %d is executed!\n", t_nr);

	mutex_lock(&lock);
	printk("mymutex - Thread %d is in critical section!\n", t_nr);
	msleep(delay[t_nr]);
	printk("mymutex - Thread %d is leaving the critical section!\n", t_nr);
	mutex_unlock(&lock);

	printk("mymutex - Thread %d finished execution!\n", t_nr);
	return 0;
}

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple example for threads in a LKM");

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("mymutex - Init threads\n");

	mutex_init(&lock);

	kthread_1 = kthread_create(thread_function, &t1, "kthread_1");
	if(kthread_1 != NULL){
		/* Let's start the thread */
		wake_up_process(kthread_1);
		printk("mymutex - Thread 1 was created and is running now!\n");
	}
	else {
		printk("mymutex - Thread 1 could not be created!\n");
		return -1;
	}
	kthread_2 = kthread_run(thread_function, &t2, "kthread_2");
	if(kthread_2 != NULL)
		printk("mymutex - Thread 2 was created and is running now!\n");
	else {
		printk("mymutex - Thread 2 could not be created!\n");
		kthread_stop(kthread_1);
		return -1;
	}
	printk("mymutex - Both threads are running now!\n");

	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("mymutex - Stop both threads\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);



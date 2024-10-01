/*
 * Credit: Johannes 4 Linux : @Johannes4Linux on GitHub
 * These modules are meant to be run on the Raspberry Pi family boards only
 *
 * Sample output:
 rpi $ dmesg
[ 1874.948046] gpio_irq:ModuleInit(): Loading module...
[ 1874.948179] gpio_irq:ModuleInit(): Done!
[ 1874.948185] gpio_irq:ModuleInit(): GPIO 17 is mapped to IRQ Nr.: 56
[ 2409.262021] gpio_irq:gpio_irq_handler(): IRQ 56 triggered!
[ 2421.193152] gpio_irq:gpio_irq_handler(): IRQ 56 triggered!
[ 2421.199330] gpio_irq:gpio_irq_handler(): IRQ 56 triggered!
[ 2421.204918] gpio_irq:gpio_irq_handler(): IRQ 56 triggered!
[ 2421.260440] gpio_irq:gpio_irq_handler(): IRQ 56 triggered!
[ 2421.266042] gpio_irq:gpio_irq_handler(): IRQ 56 triggered!
[ 2422.017707] gpio_irq:gpio_irq_handler(): IRQ 56 triggered!
...
 * How did the interrupt occur? well, you should set the line high..
 * A 'stupid' way - read risky! - is to trigger it by fiddling around GND and
 * pin 11 with a multimeter :-p
 */
#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple LKM for a gpio interrupt");

#define GPIO_LINE	17
/* GPIO 17 is phy/BCM pin 11 and is sixth pin from the 3.3V first pin on the rail
 * See https://pinout.xyz/
 */
/* below variable will hold the pin number of interrupt controller to which
 * GPIO GPIO_LINE is mapped at runtime via gpio_to_irq()
 */
unsigned int irq_number;

/**
 * @brief Interrupt service routine is called, when interrupt is triggered
 */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	pr_info("IRQ %d triggered!\n", irq);
	return IRQ_HANDLED;
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void)
{
	char buf_gpio[12];

	pr_info("Loading module... ");

	/* Setup the gpio */
	snprintf(buf_gpio, 12, "rpi-gpio-%d", GPIO_LINE);
	if(gpio_request(GPIO_LINE, buf_gpio)) {
		pr_info("Error! not allocate GPIO %d\n", GPIO_LINE);
		return -1;
	}

	/* Set GPIO GPIO_LINE direction */
	if(gpio_direction_input(GPIO_LINE)) {
		pr_info("Error!\nCan not set GPIO %d to input!\n", GPIO_LINE);
		gpio_free(GPIO_LINE);
		return -1;
	}

	/* Setup the interrupt */
	irq_number = gpio_to_irq(GPIO_LINE);
	if (irq_number < 0) {
		pr_info("Error!\nCannot allocate IRQ line\n");
		gpio_free(GPIO_LINE);
		return irq_number;
	}

	if(request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_irq", NULL) != 0){
		pr_info("Error!\nCan not request interrupt nr.: %d\n", irq_number);
		gpio_free(GPIO_LINE);
		return -1;
	}

	pr_info("Done!\n");
	pr_info("GPIO %d is mapped to IRQ Nr.: %d\n", GPIO_LINE, irq_number);
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	pr_info("gpio_irq: Unloading module... ");
	free_irq(irq_number, NULL);
	gpio_free(GPIO_LINE);
}

module_init(ModuleInit);
module_exit(ModuleExit);

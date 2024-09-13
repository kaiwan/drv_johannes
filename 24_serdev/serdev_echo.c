/* serdev_echo.c */
#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/serdev.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple loopback driver for an UART port");

/**
 * @brief Callback is called whenever a character is received
 */
static int serdev_echo_recv(struct serdev_device *serdev, const unsigned char *buffer, size_t size) {
	pr_info("Received %ld bytes with \"%s\"\n", size, buffer);
        return 	serdev_device_write_buf(serdev, buffer, size);
}

static const struct serdev_device_ops serdev_echo_ops = {
	.receive_buf = serdev_echo_recv,
	//.write_wakeup // called when ready to transmit more data
};

/**
 * @brief This function is called on loading the driver 
 */
static int serdev_echo_probe(struct serdev_device *serdev) {
	int status;
	pr_info("in the probe function!\n");

	serdev_device_set_client_ops(serdev, &serdev_echo_ops);
	status = serdev_device_open(serdev);
	if(status) {
		pr_info("Error opening serial port!\n");
		return -status;
	}

	serdev_device_set_baudrate(serdev, 9600);
	serdev_device_set_flow_control(serdev, false);
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

	status = serdev_device_write_buf(serdev, "Type something: ", sizeof("Type something: "));
	pr_info("Wrote %d bytes.\n", status);

	return 0;
}

/**
 * @brief This function is called on unloading the driver 
 */
static void serdev_echo_remove(struct serdev_device *serdev) {
	pr_info("in the remove function\n");
	serdev_device_close(serdev);
}

static struct of_device_id serdev_echo_ids[] = {
	{
		.compatible = "brightlight,echodev",
		/* identical compatible str must be in the DTS!
		 * format: "<oem>,<model/name>"
		 * Note the device 'name' is 'echodev'
		 * Our 'run' script will look for it in the DT at runtime
		 */
	}, { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, serdev_echo_ids);

static struct serdev_device_driver serdev_echo_driver = {
	.probe = serdev_echo_probe,
	.remove = serdev_echo_remove,
	.driver = {
		.name = "serdev-echo",
		.of_match_table = serdev_echo_ids,
	},
};

static int __init my_init(void) {
	pr_info("Loading the driver...\n");
	if(serdev_device_driver_register(&serdev_echo_driver)) {
		pr_info("Error! Could not load driver\n");
		return -1;
	}
	return 0;
}
static void __exit my_exit(void) {
	pr_info("Unload driver");
	serdev_device_driver_unregister(&serdev_echo_driver);
}
module_init(my_init);
module_exit(my_exit);

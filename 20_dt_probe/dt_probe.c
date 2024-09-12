/* 20_dt_probe
 */
#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple LKM to parse the device tree for a specific device and its properties");

/**
 * @brief This function is called on loading the driver 
 */
static int dt_probe(struct platform_device *pdev) {
	struct device *dev = &pdev->dev;
	const char *label;
	int my_value, ret;

	pr_info("in the probe function!\n");

	/* Check for device properties */
	if(!device_property_present(dev, "label")) {
		pr_info("Error! Device property 'label' not found!\n");
		return -1;
	}
	if(!device_property_present(dev, "my_value")) {
		pr_info("Error! Device property 'my_value' not found!\n");
		return -1;
	}

	/* Read device properties */
	ret = device_property_read_string(dev, "label", &label);
	if(ret) {
		pr_info("Error! Could not read 'label'\n");
		return -1;
	}
	pr_info("dt_probe - label: %s\n", label);
	ret = device_property_read_u32(dev, "my_value", &my_value);
	if(ret) {
		pr_info("Error! Could not read 'my_value'\n");
		return -1;
	}
	pr_info("my_value: %d\n", my_value);

	return 0;
}

/**
 * @brief This function is called on unloading the driver 
 */
static int dt_remove(struct platform_device *pdev) {
	pr_info("in the remove function\n");
	return 0;
}

static struct of_device_id my_driver_ids[] = {
	{
		.compatible = "brightlight,mydev",
	}, { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_driver_ids);

static struct platform_driver my_driver = {
	.probe = dt_probe,
	.remove = dt_remove,
	.driver = {
		.name = "my_device_driver",
		.of_match_table = my_driver_ids,
	},
};
/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void) {
	pr_info("Loading the driver...\n");
	if(platform_driver_register(&my_driver)) {
		pr_info("Error! Could not load driver\n");
		return -1;
	}
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void) {
	pr_info("Unload driver");
	platform_driver_unregister(&my_driver);
}

module_init(my_init);
module_exit(my_exit);

/* 20_dt_probe
 */
//#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__
#define dev_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/version.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple LKM to parse the device tree for a specific device and its properties");

/**
 * @brief This function is called on loading the driver
 */
static int dt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const char *label;
	int my_value, ret;

	dev_info(dev, "in the probe function!\n");

	/* Check for device properties */
	if (!device_property_present(dev, "label")) {
		dev_err(dev, "Error! Device property 'label' not found!\n");
		return -EINVAL;
	}
	if (!device_property_present(dev, "my_value")) {
		dev_err(dev, "Error! Device property 'my_value' not found!\n");
		return -EINVAL;
	}

	/* Read device properties */
	ret = device_property_read_string(dev, "label", &label);
	if (ret) {
		dev_err(dev, "Error! Could not read 'label'\n");
		return -EINVAL;
	}
	dev_info(dev, "label: %s\n", label);
	ret = device_property_read_u32(dev, "my_value", &my_value);
	if (ret) {
		dev_err(dev, "Error! Could not read 'my_value'\n");
		return -EINVAL;
	}
	dev_info(dev, "my_value: %d\n", my_value);

	return 0;
}

/**
 * @brief This function is called on unloading the driver
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 11, 0)
static int dt_remove(struct platform_device *pdev)
#else
static void dt_remove(struct platform_device *pdev)
#endif
{
	struct device *dev = &pdev->dev;

	dev_info(dev, "in the remove function\n");
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 11, 0)
	return 0;
#endif
}

static const struct of_device_id my_driver_ids[] = {
	{
	 .compatible = "mycorp,mydev",
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
static int __init my_init(void)
{
	int stat = 0;

	pr_info("Loading the driver...\n");

	// Register with the platform bus driver
	stat = platform_driver_register(&my_driver);
	if (stat < 0) {
		pr_info("Error! Could not load driver (%d)\n", stat);
		return stat;
	}
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void)
{
	pr_info("Unloading driver...");

	// Deregister with the platform bus driver
	platform_driver_unregister(&my_driver);
}

module_init(my_init);
module_exit(my_exit);

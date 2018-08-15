/*
 * bin_attr example
 *
 */
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>


static ssize_t binattr_read(struct file *file, struct kobject *kobj,
				struct bin_attribute *bin_attr, char *to,
				loff_t pos, size_t count)
{
	//char hello[15] = "hello world";
	memcpy(to, "hello world my name is", 12);
	return count;
}

static struct bin_attribute bin_attr = {
	.attr = {.name = "binattr", .mode = 0444},
	.read = binattr_read
};

static int __init example_init(void)
{
	int retval = 0;

	// Create binary file
	sysfs_create_bin_file(kernel_kobj, &bin_attr);

	return retval;
}

static void __exit example_exit(void)
{
	sysfs_remove_bin_file(kernel_kobj, &bin_attr);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Greg Kroah-Hartman <greg@kroah.com>");

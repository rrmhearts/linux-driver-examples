/*
 * Ryan McCoppin
 *
 */

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>


static int example_open(struct inode *inode, struct file *file)
{
    pr_info("Open Example Device\n");
    return 0;
}

static int example_close(struct inode *inodep, struct file *filp)
{
    pr_info("Closing Example device\n");
    return 0;
}

static ssize_t example_write(struct file *file, const char __user *buf,
		       size_t len, loff_t *ppos)
{
    pr_info("Writing %d bytes to userspace\n", len);
    return len; 
}

static ssize_t example_read(struct file *file, char __user *buf,
                       size_t len, loff_t *ppos)
{
    pr_info("Reading %d bytes from userspace\n", len);
    return len;
}


static const struct file_operations example_fops = {
    .owner			= THIS_MODULE,
    .open			= example_open,
    .release                    = example_close,
    .write			= example_write,
    .read			= example_read,
};

struct miscdevice example_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "misc_device",
    .fops = &example_fops,
};

static int __init misc_init(void)
{
    int error;

    error = misc_register(&example_device);
    if (error) {
        pr_err("Unable to register Example Driver\n");
        return error;
    }

    pr_info("Registered Example Driver\n");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&example_device);
    pr_info("Deregister Example Driver\n");
}

module_init(misc_init)
module_exit(misc_exit)

MODULE_DESCRIPTION("Misc Driver Example");
MODULE_AUTHOR("Ryan McCoppin <rrmhearts@gmail.com>");
MODULE_LICENSE("GPL");

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "16c750_support.h"


#define UART2_BASE 0x48024000
#define UART2_SIZE 4096
int loopback_state = 0;
void __iomem * my_cookie;


int uart_open(struct inode *inode, struct file *file)
{
	printk("Open from userspace\n");
	return 0;
}

int uart_release(struct inode *inode, struct file *file)
{
	printk("Release from userspace\n");
	return 0;
}

ssize_t uart_write(struct file *file, const char __user * buf, size_t count, loff_t *ppos)
{
	int i=0;
	int count_uncopied;
	char *receive_data = kzalloc(sizeof(char)*count, GFP_KERNEL
);

	printk("Write from userspace\n");
	count_uncopied = copy_from_user(receive_data, buf, count);
	printk("count uncopied: %d\n", count_uncopied);
	printk("Writing 'data' to THR_REG\n");

	while (i < count)
	{
		write_uart_reg( UART_THR_REG, receive_data[i]);
		i++;
	}
	return i;
}

ssize_t uart_read( struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	char sending_data[count];
	int ret, i=0, size_reg;

	printk("Read from userspace\n");
	size_reg = read_uart_reg(UART_RXFIFO_LVL_REG);
	while (size_reg > 0)
	{
		sending_data[i] = read_uart_reg(UART_RHR_REG);
		size_reg--;
		i++;
	}
	ret = copy_to_user(buf, sending_data, i);
	return i;
}


static const struct file_operations uart_loop_device_fops = {
	.read = uart_read,
	.write = uart_write,
	.open = uart_open,
	.release = uart_release,
};

static struct miscdevice uart_loop_device = {
	MISC_DYNAMIC_MINOR, "uart_loop", &uart_loop_device_fops
};

static ssize_t loopback_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	printk("loopback_show\n");
	if (loopback_state)
		return sprintf(buf, "on\n");
	else
		return sprintf(buf, "off\n");
}

static ssize_t loopback_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	printk("loopback_store\n");
	if (!strcmp(buf, "on\n") )
		loopback_state = 1;
	else
		loopback_state = 0;

	return count;
}
DEVICE_ATTR(loopback, 0644, loopback_show, loopback_store);

// This function writes a 16-bit value to the UART register with byte offset reg_offset 
void write_uart_reg_raw (u32 reg_offset, u16 value)
{
	printk("write_uart_reg_raw\n");
	iowrite16(value, my_cookie+reg_offset);
}

// This function reads a 16-bit value from the UART register with the byte offset reg_offset
u16 read_uart_reg_raw (u32 reg_offset)
{
	printk("read_uart_reg_raw\n");
	return ioread16(my_cookie+reg_offset);
}

static void uart_init(void)
{
	printk("uart_init\n");
	my_cookie = ioremap_nocache(UART2_BASE, UART2_SIZE);
	if (!my_cookie) {
		//res = -EBUSY;
		printk("Utter Failure: uart_init\n");
	}

	init_uart_reg ();

	write_uart_reg( UART_SYSC_REG, 0x0002);
	while ( (0x0001 & read_uart_reg (UART_SYSS_REG)) != 1)
	{
		printk("uart_init: Not reset (0) yet!");
	}
	write_uart_reg( UART_MCR_REG,  0x0010);
	write_uart_reg( UART_FCR_REG,  0x0007);
	write_uart_reg( UART_LCR_REG,  0x0003);
	write_uart_reg( UART_DLH_REG,  0x0001);

	write_uart_reg( UART_MCR_REG,  0x0017);
	write_uart_reg( UART_MDR1_REG, 0x0000);

	printk("Contents of MCR_REG: %x\n", read_uart_reg( UART_MCR_REG) );
	
	printk("Writing 'A' to THR_REG\n");
	write_uart_reg( UART_THR_REG, 0x41);
	while ( (0x0001 & read_uart_reg (UART_RXFIFO_LVL_REG)) != 1)
	{
		printk("uart_init: waiting for response...");
	}
	printk("Reading 'A' from RHR_REG\n");
	printk("Value: %x\n", read_uart_reg( UART_RHR_REG ) );
}

static int uart_loop_probe_callback_fn(struct platform_device *pdev)
{
	int ret;
	printk("Binding/Initializing probe callback ~Ryan\n");

	ret = device_create_file(&pdev->dev, &dev_attr_loopback);

	uart_init();

	// Shows parent in /sys to be the device. Not required.
	uart_loop_device.parent = &pdev->dev;

	ret = misc_register(&uart_loop_device);
	if (ret < 0)
		pr_err("uart_loop: error %d registering device\n", ret);
	
	return 0;
}


static void uart_deinit(void)
{
	iounmap(my_cookie);
	misc_deregister(&uart_loop_device);
}
static int uart_loop_remove_callback_fn(struct platform_device *pdev)
{
	printk("Unbinding/Removing probe callback ~Ryan\n");
	device_remove_file(&pdev->dev, &dev_attr_loopback);
	uart_deinit();

	return 0;
}
//         major=$(awk -v mod="$module" '$2 == mod {print $1}' /proc/devices)
//         mknod /dev/sample-driver c $major 0

static struct platform_driver uart_loop_driver = {
	.probe = uart_loop_probe_callback_fn,
	.remove = uart_loop_remove_callback_fn,
	.driver = {
		.name = "barrometer_uart2",
		.owner = THIS_MODULE,
	},
};

unsigned long bps_rate = 115200;

module_param(bps_rate, ulong, 0444);

static int __init init_callback_fn(void)
{
	printk("Initializing uart_loop ~Ryan\n");
	printk("BPS rate is %lu\n", bps_rate);

	return platform_driver_register(&uart_loop_driver);
}

static void __exit exit_callback_fn(void)
{
	printk("Exiting uart_loop ~Ryan\n");
	platform_driver_unregister(&uart_loop_driver);
}


MODULE_AUTHOR("Ryan McCoppin <rrmhearts@gmail.com>");
MODULE_DESCRIPTION("uart loop");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

module_init(init_callback_fn);
module_exit(exit_callback_fn);

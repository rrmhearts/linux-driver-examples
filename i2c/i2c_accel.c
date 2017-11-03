#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/i2c.h>


#define WHO_AM_I  0x0F //32
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define STATUS    0x27
#define OUT_X_L   0x28
#define OUT_X_H   0x29
#define OUT_Y_L   0x2A
#define OUT_Y_H   0x2B
#define OUT_Z_L   0x2C
#define OUT_Z_H   0x2D

static struct i2c_client *i2c_accel_client;

// .open
int i2c_open(struct inode *inode, struct file *file)
{
	printk("Open from userspace\n");
	return 0;
}

// .release
int i2c_release(struct inode *inode, struct file *file)
{
	printk("Release from userspace\n");
	return 0;
}

// .write
ssize_t i2c_write(struct file *file, const char __user * buf, size_t count, loff_t *ppos)
{
	//int i=0;
	//int count_uncopied;
	//char *receive_data = kzalloc(sizeof(char)*count, GFP_KERNEL);

	printk("Write from userspace\n");
	return 0;

}

// .read
ssize_t i2c_read( struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	//char sending_data[count];
	//int ret, i=0, size_reg;

	printk("Read from userspace\n");
/*
	size_reg = read_i2c_reg(UART_RXFIFO_LVL_REG);
	while (size_reg > 0)
	{
		sending_data[i] = read_i2c_reg(UART_RHR_REG);
		size_reg--;
		i++;
	}
	ret = copy_to_user(buf, sending_data, i);
	return i;
*/
	return 0;
}


static const struct file_operations i2c_accel_device_fops = {
	.read = i2c_read,
	.write = i2c_write,
	.open = i2c_open,
	.release = i2c_release,
};

static struct miscdevice i2c_accel_device = {
	MISC_DYNAMIC_MINOR, "i2c_accel", &i2c_accel_device_fops
};

// root@android:/sys/bus/i2c/devices/1-0018 # cat accel  
static ssize_t accel_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	// sysfs accel
	unsigned short xl, xh, yl, yh, zl, zh;
	unsigned short ux, uy, uz;
	short x, y, z;

	printk("accel_show\n");

	xl = i2c_smbus_read_byte_data(i2c_accel_client, OUT_X_L);
	xh = i2c_smbus_read_byte_data(i2c_accel_client, OUT_X_H);
	ux = (xh << 8) | xl;
        x = (short)ux;

	yl = i2c_smbus_read_byte_data(i2c_accel_client, OUT_Y_L);
	yh = i2c_smbus_read_byte_data(i2c_accel_client, OUT_Y_H);
	uy = (yh << 8) | yl;
        y = (short)uy;

	zl = i2c_smbus_read_byte_data(i2c_accel_client, OUT_Z_L);
	zh = i2c_smbus_read_byte_data(i2c_accel_client, OUT_Z_H);
	uz = (zh << 8) | zl;
        z = (short)uz;

	printk("x,y,z data: %d,%d,%d\n", x, y, z);

       // fx = (x * 2.0) / 32768.0;

        return(sprintf(buf,"%d,%d,%d\n", x, y, z));
}

static ssize_t accel_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	// sysfs accel
	printk("accel_store\n");
	return count;
}

DEVICE_ATTR(accel, 0644, accel_show, accel_store);

// root@android:/sys/bus/i2c/devices/1-0018 # cat accel  

static void i2c_init(void)
{
	unsigned char whoami;

	printk("i2c_init\n");

	whoami = i2c_smbus_read_byte_data(i2c_accel_client, WHO_AM_I);
	printk("WHO AM I?? %x\n", (int)whoami);

	i2c_smbus_write_byte_data(i2c_accel_client, CTRL_REG1, 0x27);

	while ( (i2c_smbus_read_byte_data(i2c_accel_client, STATUS) & 0x08) != 0x8) 
	{ printk("."); }

	printk("\n");
}

// .probe module 
static int i2c_accel_probe_callback_fn(struct i2c_client *client,
	    const struct i2c_device_id *id)
{
	int ret;
	printk("I2C ACCEL: Binding/Initializing probe callback ~Ryan\n");
	i2c_accel_client = client;
	
	ret = device_create_file(&client->dev, &dev_attr_accel);

	i2c_init();

	// Shows parent in /sys to be the device. Not required.
	i2c_accel_device.parent = &client->dev;

	ret = misc_register(&i2c_accel_device);
	if (ret < 0)
		pr_err("i2c_loop: error %d registering device\n", ret);
	
	return 0;
}


static void i2c_deinit(void)
{

	misc_deregister(&i2c_accel_device);
}

// .remove module
static int i2c_accel_remove_callback_fn(struct i2c_client *client)
{
	printk("I2C ACCEL: Unbinding/Removing probe callback ~Ryan\n");
	device_remove_file(&client->dev, &dev_attr_accel);
	i2c_deinit();

	return 0;
}
static const struct i2c_device_id accel_i2c_id[] = {
	{"i2c_accel", 0 },
	{ }
};

static struct i2c_driver i2c_accel_driver = {
	.probe = i2c_accel_probe_callback_fn,
	.remove = i2c_accel_remove_callback_fn,
	.driver = {
		.name = "i2c_accel",
		.owner = THIS_MODULE,
	},
	.id_table = accel_i2c_id,
};

static int __init init_callback_fn(void)
{
	int ret;
	printk("Initializing i2c_loop ~Ryan\n");
	//printk("BPS rate is %lu\n", bps_rate);

	ret = i2c_add_driver(&i2c_accel_driver);
	if (ret != 0)
		pr_err("Failed to register TPS65912 I2C driver: %d\n", ret);

	return ret;
	//return platform_driver_register(&i2c_accel_driver);
}

static void __exit exit_callback_fn(void)
{
	printk("Exiting i2c_loop ~Ryan\n");
	i2c_del_driver(&i2c_accel_driver);
	//platform_driver_unregister(&i2c_accel_driver);
}


MODULE_AUTHOR("Ryan McCoppin <rrmhearts@gmail.com>");
MODULE_DESCRIPTION("i2c loop");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

module_init(init_callback_fn);
module_exit(exit_callback_fn);

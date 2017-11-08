# I2C on Linux
The i2c subsystem allows Linux to be master and all connected devices to be slaves. They interact on the i2c bus.

When we talk about I2C, we use the following terms:

  **Bus**    -> *Algorithm*, 
                *Adapter*

  **Device** -> *Driver*, 
                *Client*
## How does a I2C device differ from a platform device?
An i2c device is an example of a platform device, however the complexities of i2c protocol are abstracted in a piece of hardware called the *i2c controller* which provides a way to connect and communicate to i2c devices. 
The driver used for configuring and using the i2c controller is a *platform driver*. The controller is an example of a platform device..

If an i2c device does not fit into the i2c controller model... may have to create it as a platform driver and customize for device.?

## I2C Algorithm

An Algorithm driver contains general code that can be used for a whole class of I2C adapters. 
It's function is to read and write the I2C messages to the hardware. 
Each specific adapter driver either depends on one algorithm driver, or includes its own implementation. 
There are three algorithms defined for the bus: pca, pcf and bitbanging. 
Bigbanging involves GPIO lines where the others write to an I2C controller chip. (Is the *I2c controller* required?)

**I2C algorithms** are used to communicate with devices when the driver requests to write or read data from the device. 
They are represented by the structure `i2c_algorithm` which allows you to define function pointers that can write I2C messages (`master_xfer`) or SMBus messages (`smbus_xfer`). 


## I2C Adapter
There are multiple buses on the board and each bus is represented to Linux by a `struct i2c_adapter` (include/linux/i2c.h) 
Each bus in the system is represented by a *bus number* connecting a bus to a i2c adapter structure. 
The bus is subject to a protocol called the *i2c algorithm* for communicating with the device. 
This bus may be used with I2C messages or SMBus (System Management Bus) messsages. Modern pcs rely on SMBus to connect devices such as RAM modules, i2c devices, and others. The SMBus is considered a subset of the I2C protocol. 

If there is a system with 3 I2C buses, two controllowed by a *controller chip* and one *big-banged*, there would be 3 instances of `i2c_adapter` and 2 instances of an `i2c_algorithm`.

## I2C Client
Each *device* connected to the bus is represented by the `struct i2c_client` (include/linux/i2c.h). This **maybe** defined beforehand in board-related code (arch/arm/mach.../board-omap...c). 
The device has an *address* that is used by driver to determine where the device is on the bus. This address is hardcoded by the device? It's something like 0x14. The device also has a *name* and an *interrupt number* used to trigger and interrupt. 

You must tell the *client* about the *adapter* which represents its bus line. When you want to read or write to device, it must know which bus line to use for communication. The *client* represents the device to linux. Since the device is hardware, this is usually defined under [Board Information](###I2C-Board-Information)

### I2C Board Information
Again, this may be where the `i2c_client` is defined in `struct i2c_board_info` (include/linux/i2c.h). 
The board information structure contains information related to the board as well as devices on the board? The *type* field indicates the type of (i2c) device and is copied to the `i2c_client` object. The *addr* field is similar to above in the client (copied to address field in *i2c_client*. There is also parallels *irq* .. The board information structS are written as an array of devices like the following:
```
static struct i2c_board_info z23_devices[] = {
	{
		.type = eeprom_abc,
		.addr = 0x28,
		.irq = 5,
	},
	{
		.type = adc_efg,
		.addr = 0x29,
	},
};
```
This board info is received into the kernel during bootup. The *i2c_client* is created when the *i2c_adapter* is registered. It is possible to manually add *i2c_clients* that the board does not know about.?

## I2C Driver
For each device, there exists a driver that corresponds to it. The driver is represented by `struct i2c_driver` (include/linux/i2c.h). 
The driver has a *name* which is used to link the client device with one driver. The driver also has a *probe* function which is called when the device and driver are both found on the system by a *Linux device driver subsystem*. For example,
```
static struct i2c_driver adc_driver = {
	.driver = {
		.name = adc_efg,
		.owner = THIS_MODULE,
	},
	.probe = adc_probe,
};
```

Below is an image of the I2C subsystem for reference. This image is most helpful for reference and understanding the system as a whole.
![](https://github.com/rrmhearts/linux-driver-examples/blob/master/i2c/etc/linux_i2c_subsystem.jpg)

## I2C Device Registration

If the bus number on which the device is connected is known use,

```
int i2c_register_board_info(int busnum, struct i2c_board_info &ast;info, unsigned len);
```

where *busnum* is the number of the bus which the device is connected (identify the *i2c_adapter*); info is an array of *i2c_board_info* structures and *len* is the array length..

Else if the bus number is not known but the *i2c_adapter* is known, use,

```
struct i2c_client &ast;
i2c_new_device(struct i2c_adapter &ast;adap, struct i2c_board_info const *info);
```

where info is an object, indexed of the previous array (adap, &z23_devices[1]).

Also, the `struct i2c_driver` has to be registered with the I2C subsytem in the `module_init`
```
i2c_add_driver(struct i2c_driver *drv);
```
This line will match the name of the driver through the i2c subsytem to all the i2c_client names; on a match, the probe routine of the driver will be called and the *client* will verified as a device (in probe). 

## Communicating on the I2C Bus

### Reading bytes from bus
```
i2c_smbus_read_byte_data(struct i2c_client *client, u8 command);
```
Client is received in probe function...
And reading words is similar:
```
i2c_smbus_read_word_data(struct i2c_client *client, u8 command);
```

### Writing bytes
```
i2c_smbus_write_byte_data(struct i2c_client *client, u8 command, u8 data);

i2c_smbus_write_word_data(struct i2c_client *client, u8 command, u16 data);
```

## Instantiating I2C devices
There are several means of declaring/instantiating a I2C device

1. Declare device by bus number in `i2c_board_info` found in arch/ board code
```
static struct i2c_board_info h4_i2c_board_info[] __initdata = {
   {
           I2C_BOARD_INFO("isp1301_omap", 0x2d),
           .irq            = OMAP_GPIO_IRQ(125),
   },
   {       /* EEPROM on mainboard */
           I2C_BOARD_INFO("24c01", 0x52),
           .platform_data  = &m24c01,
   },
   {       /* EEPROM on cpu card */
           I2C_BOARD_INFO("24c01", 0x57),
           .platform_data  = &m24c01,
   },
};

static void __init omap_h4_init(void)
{
       (...)
        i2c_register_board_info(1, h4_i2c_board_info,
                       ARRAY_SIZE(h4_i2c_board_info));
       (...)
}
```

2. Declare device via device tree (dts) and will be imported into kernel

```
i2c1: i2c@400a0000 {
	clock-frequency = <100000>;
	...
```
3. Declare device explicitly in module
Usually when you don't know the i2c bus number ahead of time or for internal communication. 
You include the `i2c_board_info` within the module and then call `i2c_new_device()` to register device with system.
```
static struct i2c_board_info sfe4001_hwmon_info = {
        I2C_BOARD_INFO("max6647", 0x4e),
};

int sfe4001_init(struct efx_nic *efx)
{
        (...)
        efx->board_info.hwmon_client =
                i2c_new_device(&efx->i2c_adap, &sfe4001_hwmon_info);

        (...)
}
```

4. Probe I2C bus for devices
This method allows i2c-core to probe for devices after their drivers are loaded. If one is found, an I2C device will be initiated.
The driver must implement the "detect()" method which identifies a supported device by reading arbitrary registers. 
This method will only probe buses that are likely to have supported devices and *agree* to be probed.

5. Lastly, you can instantiate a device from user space through sysfs
This is only if you cannot modify the kernel. You need to know the name of the I2C device and the address.
**Example**
```
$ echo eeprom 0x50 > /sys/bus/i2c/devices/i2c-3/new_device
```
This method can be used to correct mistaken addresses and such in dts or kernel. 
Unexpected address. Unsupported devices. Development issues.








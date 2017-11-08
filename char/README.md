# Character Device Drivers

## Overview
Character devices are those who send and receive single characters at a time.
Some examples include serial ports, parallel ports, sounds cards.

I2C device files are character device files with major device number 89
and a minor device number corresponding to the number assigned (by the operating system).
In order to converse with I2C character devices, you go through `#include <linux/i2c-dev.h>`
You can use the i2c-tools `i2cdetect -l` to determine the adapter number.
You also will need the I2C address in your program for communicating (e.g. `ioctl(file, I2C_SLAVE, addr)`).
SMBus commands or plain I2C is then used to communicate with device. 
You can find more information and example under [I2C devices](https://github.com/rrmhearts/linux-driver-examples/tree/master/i2c).

## Code changes
The following may eventually be integrated into the character driver example code. A newer initialization for character drivers is as follows:

```
dev_t dev_num;

static int __init my_init(void)
{
    int i;
    dev_t curr_dev;

    /* Request the kernel for N_MINOR devices */
    alloc_chrdev_region(&dev_num, 0, N_MINORS, "my_driver");

    /* Create a class : appears at /sys/class */
    my_class = class_create(THIS_MODULE, "my_driver_class");

    /* Initialize and create each of the device(cdev) */
    for (i = 0; i < N_MINORS; i++) {

        /* Associate the cdev with a set of file_operations */
        cdev_init(&my_cdev[i], &fops);

        /* Build up the current device number. To be used further */
        curr_dev = MKDEV(MAJOR(dev_num), MINOR(dev_num) + i);

        /* Create a device node for this device. Look, the class is
         * being used here. The same class is associated with N_MINOR
         * devices. Once the function returns, device nodes will be
         * created as /dev/my_dev0, /dev/my_dev1,... You can also view
         * the devices under /sys/class/my_driver_class.
         */
        device_create(my_class, NULL, curr_dev, NULL, "my_dev%d", i);

        /* Now make the device live for the users to access */
        cdev_add(&my_cdev[i], curr_dev, 1); 
    }

    return 0;
}
```

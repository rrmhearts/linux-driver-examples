# Character Device Drivers

A **character device driver** is a type of Linux device driver that provides unbuffered, sequential access to hardware devices. These drivers allow user applications to interact with hardware through standard file operations like `open()`, `read()`, `write()`, and `ioctl()` by exposing a device file under `/dev/`, such as `/dev/i2c-1`.

In the I²C subsystem, the `i2c-dev` kernel module provides a **character device interface** for I²C adapters. This lets userspace applications communicate with I²C devices without writing custom kernel drivers.

To enable this:

1. Load the module:

   ```sh
   modprobe i2c-dev
   ```

2. Confirm the presence of I²C character devices:

   ```sh
   ls /dev/i2c-*
   ```

3. Use open/read/write/ioctl system calls to communicate with devices using the `/dev/i2c-*` interface (as shown in the [Userspace Development](#userspace-development) section).

This abstraction is especially useful for prototyping, testing, and interfacing with simple I²C peripherals like EEPROMs, sensors, or ADCs, without writing a full kernel driver.

> Note: Character device drivers are distinct from platform or bus drivers. They provide a generic, file-based interface to user programs, while bus and platform drivers manage hardware enumeration and initialization at the kernel level.

## A Reduction

Character devices are those who send and receive single "character" at a time.
Some examples include serial ports, parallel ports, sounds cards.

In general, I²C drivers themselves are not character device drivers, but they can expose a character device interface to userspace. `i2c-dev` module exposes I²C *adapters* (buses) to userspace called a device file, e.g. `/dev/i2c-0`. I²C device files are character device files with major device number 89 and a minor device number corresponding to the number assigned (by the operating system).
In order to converse with I²C character devices, you go through `#include <linux/i2c-dev.h>`
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

Great question — and the answer is **not exactly, but sometimes**. Here's a more precise breakdown:

---

## Are I²C Drivers Character Drivers?

**In general, I²C drivers themselves are *not* character device drivers**, but they can **expose a character device interface** to userspace.

### Breakdown:

### 1. **I²C Drivers (Core Perspective)**

* I²C device drivers (implementing `struct i2c_driver`) manage communication with specific I²C slave devices like sensors or EEPROMs.
* These drivers operate at the **kernel level**, and work with the I²C subsystem using `i2c_transfer()` or SMBus helper functions.
* They typically don’t provide a device file (`/dev/xxx`) on their own.

### 2. **Character Device Interface (Optional via `i2c-dev`)**

* The **`i2c-dev` module** exposes I²C **adapters** (i.e. buses) to userspace as character devices: `/dev/i2c-0`, `/dev/i2c-1`, etc.
* This is a **character driver** registered with the kernel using `misc_register()` or `cdev`, and backed by `file_operations`.
* It enables user-space code to interact with I²C clients by manually crafting transactions using `ioctl()` and `read()`/`write()` calls.

### Summary

| Driver Type        | Exposes /dev entry | Interacts with I²C devices | Used in kernel or user space |
| ------------------ | ------------------ | -------------------------- | ---------------------------- |
| I²C Device Driver  | ❌ (by default)     | ✅                          | Kernel space                 |
| i2c-dev (char drv) | ✅ (`/dev/i2c-X`)   | ✅                          | User space                   |

---

### When Would You Write a Character Driver for I²C?

If you're writing a **custom driver** for a specific I²C device and want to expose a **clean, device-specific interface to userspace** (e.g., `/dev/temperature_sensor0`), you *can* combine:

* An I²C device driver (to talk to the hardware),
* With a **character driver** interface (using `register_chrdev_region()` or `misc_register()`),
  to make it easier for applications to use the device.

This pattern is common in kernel modules that abstract a hardware device behind a familiar file interface.

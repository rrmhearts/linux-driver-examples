# Linux Drivers Overview

This is meant to provide documentation on linux drivers, specifically as an all in one knowledge base. Documentation is sparse and spread out across the internet at present.

Examples contained
1. char  
2. misc
3. platform
4. i2c

What is a kobject? see kobject/

## What is a misc driver?
Used to interact with user space. Some drivers may be broken into parts, a i2c or platform driver to interact with a device and a misc driver to interact with userspace.. Misc drivers can be registered in platform's probe function.

Misc facilitates user space read and write calls..

Misc drivers do not require major numbers and provide only 1 minor number. (drivers/char/misc && misc_register)

There is also (drivers/misc) which are a set of drivers that do not fit in other categories.. Two different topics.

## What is a platform device?
Platform devices are inherently *not discoverable* They cannot inform software of their presence. i2c devices fall into this category. The software must know at compile time of their existence (via `board_info` or device tree (dts)..)

They are bound to drivers by *name matching* which you can learn in the i2c/ directory. Should be registered asap so that they can used.

USB and PCI would then **not** count as platform devices.

There are 2 requirements to work with platform devices
1. **registering the driver by name**
2. **registering teh device using the same name as the driver**

Notice these things about platform drivers

1. register and interrupt addresses are hardcoded in the device tree, which represents the SoC
2. there is no way to remove the device hardware (since it is part of the SoC)
3. the correct driver is selected by the `compatible` device tree property which matches platform_driver.name in the driver
    platform_driver_register is the main register interface
e.g.
```
		lkmc_platform_device@101e9000 {
			compatible = "lkmc_platform_device";
			reg = <0x101e9000 0x1000>;
			interrupts = <18>;
			interrupt-controller;
			#interrupt-cells = <2>;
			clocks = <&pclk>;
			clock-names = "apb_pclk";
			lkmc-asdf = <0x12345678>;
};
```

## What is a non-platform device (USB, PCI)?

Non-platform devices such as PCI are inherently *discoverable*. This means that software can **find** new devices added to the system during runtime.

Notice these things about non-platform drivers

1. register and interrupt addresses are dynamically allocated by the PCI system, no device tree is used
2. the correct driver is selected by the PCI vendor:device ID. This is baked into every device, and vendors must ensure uniqueness.
3. we can insert and remove the PCI device with device_add edu and device_del edu as we can in real life. Probing is not automatic, but can be done after boot with echo 1 > /sys/bus/pci/rescan.

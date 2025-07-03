# Linux Device Drivers Overview

This document provides a consolidated introduction to **Linux device drivers**, serving as a centralized knowledge base. Since Linux driver documentation is often fragmented across various sources, this guide aims to provide a clear, cohesive overview — with a particular focus on **platform**, **I2C**, **misc**, and **character** drivers.

## Contents & Examples

Keep scrolling for summary information for each topic. This repository contains example drivers and related documentation:

1. [Character Drivers](./char/)
2. [Misc Drivers](./misc/)
3. [Platform Drivers](./platform/)
4. [I2C Drivers](./i2c/)
5. [WSL Notes](./wsl/README.md)
6. [What is a kobject?](./sysfs/kobject/)
7. [Coffee](#coffee-is-a-proven-love-language)

## I2C Devices and `i2cdetect`

I2C devices communicate over a **two-wire bus** (SDA + SCL) and are typically **not discoverable** in the same way USB or PCI devices are. In Linux, I2C devices are treated as **platform devices** — meaning the system must already know they exist (via device tree or static registration).

To inspect the system for available I2C buses and devices, use the [`i2cdetect`](https://linux.die.net/man/8/i2cdetect) tool:

```bash
i2cdetect -l       # list I2C buses
i2cdetect -y <bus> # scan for devices on a specific bus
```

## What Is a Misc Driver?

A **misc driver** is a simple type of character driver used to expose hardware functionality to **user space**. These drivers are often used alongside platform or I2C drivers to provide userspace interfaces via `/dev`.


### Key Properties:

* Registered using `misc_register()`
* Allocated a **single minor number** from the misc major (no need to manually allocate major numbers)
* Typically exposes `read`, `write`, `ioctl`, and `poll` file operations
* Lives under:

  * `drivers/char/misc.c` — registration API
  * `drivers/misc/` — actual drivers that don’t fit other categories

### Use Case:

A misc driver can be registered inside a platform driver's `probe()` function to expose the device to user space.

> ⚠️ Note: Don't confuse `misc_register` (API) with the `drivers/misc/` directory — they are related but not the same.


## What Is a Platform Device?

**Platform devices** represent hardware that **cannot self-describe or announce their presence** to the operating system. Examples include on-chip peripherals (UART, I2C, GPIO), especially common in embedded systems and SoCs.

Generally, they are a foundation for running software and a crucial part of a system's architecture, providing the basic functionality needed for other software and hardware to operate. Platform devices are typically integrated into a system-on-chip (SoC) and can be directly addressed by the CPU. 

### Key Properties:

* Not dynamically discovered — must be known at **compile-time**
* Registered via:

  * **Device Tree** (preferred for ARM/embedded): `.dts` files
  * **Static code registration**: `platform_device_register()`
* Bound to a driver using **name matching** (`compatible` in DT or `.name` in code)

### Platform Driver Requirements:

1. A device must be registered using `platform_device_register()` or Device Tree
2. A driver must be registered using `platform_driver_register()`
3. The names must match via `compatible` strings or driver `.name`

### Device Tree Example:

```dts
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

### Notes:

* Device memory regions, IRQs, and clocks are hardcoded via Device Tree
* These devices **cannot be hotplugged**
* The correct driver is matched via the `compatible` string in the Device Tree and the driver's `.of_match_table`


## What Is a Non-Platform Device?

Devices like **PCI**, **USB**, and **virtio** are **discoverable** — meaning the OS can detect them at runtime without prior knowledge.

### Key Properties:

* Auto-discovered by hardware buses (PCI, USB)
* Register and interrupt addresses are **dynamically assigned**
* Devices are matched to drivers using:

  * **PCI Vendor\:Device ID**
  * **USB Vendor/Product ID**
* Device hotplug is supported (can be added or removed at runtime)

### PCI Matching Flow:

* PCI core enumerates the device and matches the IDs with a driver table (`pci_device_id`)
* Driver is probed automatically when a match is found
* Devices can be dynamically rescanned with:

```bash
echo 1 > /sys/bus/pci/rescan
```

### Example Matching Entry (C++/C):

```c
static const struct pci_device_id my_pci_ids[] = {
    { PCI_DEVICE(0x1234, 0x5678) }, // Vendor ID, Device ID
    { 0, }
};
MODULE_DEVICE_TABLE(pci, my_pci_ids);
```

## Summary: Platform vs Non-Platform

| Feature            | Platform Device         | Non-Platform Device (PCI/USB) |
| ------------------ | ----------------------- | ----------------------------- |
| Discovery          | Static (manual)         | Automatic (hotplug capable)   |
| Typical Use Case   | SoC peripherals         | Add-in cards, USB peripherals |
| Address Assignment | Hardcoded (Device Tree) | Dynamic (PCI/USB assigns)     |
| Driver Matching    | Name / compatible       | Vendor\:Device ID             |
| Hotplug Support    | ❌ Not possible          | ✅ Yes                         |

## Coffee is a proven love language

If this has proven helpful to you:

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.paypal.com/paypalme/rrmhearts)


# Introduction to Platform Device Drivers

Platform device drivers in Linux are designed to support hardware that is **not discoverable at runtime**. Unlike buses such as PCI or USB—where devices announce themselves and are dynamically enumerated—platform devices require the operating system to **already know about their existence**. This characteristic makes them common in embedded systems and System-on-Chip (SoC) designs, where hardware is tightly integrated and static in nature.

A typical example of a platform device is a **UART (Universal Asynchronous Receiver/Transmitter)** controller built directly into the SoC. The operating system cannot query the hardware to find out if a UART exists or where its registers are located. Instead, this information must be supplied by the system firmware—either statically through board files (in older systems) or through a Device Tree or ACPI tables (in modern systems).

Platform drivers are responsible for handling the lifecycle and functionality of these fixed-function devices. They are bound to platform devices via **string-based name matching**, most often using the `compatible` property defined in the Device Tree. Once a match is found, the driver's `probe()` function is called to initialize the device.

This driver model provides a clean abstraction for non-discoverable peripherals, allowing Linux to manage devices like UARTs, timers, GPIO controllers, and I2C adapters in a uniform way, even though they are deeply embedded and fixed into the hardware platform.

Understanding platform devices and their drivers is fundamental when working on kernel modules for embedded Linux systems, particularly where hardware resources must be managed with precision and knowledge of the underlying board layout.

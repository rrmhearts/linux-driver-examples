# Introduction to `sysfs`

`sysfs` is a **virtual filesystem** in Linux that exposes kernel objects and their attributes to **userspace** in a hierarchical, human-readable form. It allows user applications and developers to **query and control kernel subsystems**, devices, and driver internals using standard file operations like `read()` and `write()`.

`sysfs` is typically mounted at `/sys`:

```bash
$ mount | grep sysfs
sysfs on /sys type sysfs (rw,nosuid,nodev,noexec,relatime)
```

## Purpose

The main goals of `sysfs` are:

* To expose internal kernel objects and relationships to userspace
* To allow **fine-grained device and driver control**
* To provide a **clean alternative** to deprecated interfaces like `/proc`

## How It Works

`sysfs` represents kernel objects using a special internal structure called a [`kobject`](https://elixir.bootlin.com/linux/latest/source/include/linux/kobject.h). These `kobjects` organize into a hierarchy of `ksets` (collections of related objects), and each kobject gets a directory under `/sys`.

Each file in a `sysfs` directory maps to a **kernel attribute**, typically represented by a `show` (read) and `store` (write) function.

For example, if you have a device registered via the kernel’s device model, it may appear as:

```
/sys/class/net/eth0/
  ├── address
  ├── speed
  └── mtu
```

Each of these files can be read or written to via userspace, providing a consistent way to interact with kernel internals.

## Structure

* **/sys/class/** – High-level categories like `net`, `power`, `graphics`, etc.
* **/sys/devices/** – Physical devices arranged in topology (e.g. PCI, platform)
* **/sys/bus/** – Bus subsystems like `i2c`, `usb`, `spi`
* **/sys/module/** – Loaded kernel modules and their parameters
* **/sys/firmware/** – Platform firmware info (ACPI, EFI, etc.)

## Common Use Cases

* Adjusting driver parameters at runtime
* Monitoring hardware status
* Debugging device and driver states
* Exposing custom device attributes from kernel modules

## Developer Perspective

From a kernel developer’s standpoint, `sysfs` is often used in drivers to:

* Expose internal state variables or statistics
* Provide interfaces for configuration or tuning
* Debug and test device behavior

This is done using helper APIs like:

```c
device_create_file()
sysfs_create_file()
kobject_create_and_add()
```

These APIs allow kernel code to define readable and/or writable attributes that are automatically exported to `/sys`.

## Summary

| Feature      | Description                             |
| ------------ | --------------------------------------- |
| Filesystem   | Virtual (in-memory), mounted at `/sys`  |
| Purpose      | Expose kernel object model to userspace |
| Backed by    | `kobject` and `kset` structures         |
| Key usage    | Driver configuration, introspection     |
| Accessed via | Standard file I/O (`cat`, `echo`, etc.) |

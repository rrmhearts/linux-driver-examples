# kobjects

A **`kobject`** (short for **kernel object**) is a fundamental building block in the Linux kernel that represents kernel entities in a unified and structured way — especially for interacting with **`sysfs`**.


## What Is a `kobject`?

In the context of **`sysfs`**, a `kobject` serves as a **bridge between kernel data structures and the userspace filesystem**. It enables structured representation of kernel components (devices, modules, buses, etc.) in the `/sys` virtual filesystem.

It provides:

* **Naming and reference counting**
* **A place in a hierarchy** (parents and children)
* **Sysfs representation** (directories and attributes)


## How Does It Work?

When a `kobject` is created and initialized, it automatically creates a **directory under `/sys`**, named after the `kobject`. Inside this directory, it can expose attributes (files) that represent kernel data. These are typically created via `sysfs_create_file()` or using higher-level wrappers like `device_create_file()`.

### Example

```c
struct kobject *kobj;

kobj = kobject_create_and_add("my_kobj", kernel_kobj);
if (!kobj)
    return -ENOMEM;

sysfs_create_file(kobj, &my_attr.attr);
```

This would create:

```
/sys/kernel/my_kobj/
└── my_attr
```

Where `my_attr` could be a file that userspace can read/write, backed by functions defined in your code.


## Where Do You See `kobject`?

* **Devices**: `struct device` embeds a `kobject`, so every device shows up in `/sys/devices/...`
* **Modules**: `/sys/module/` entries are backed by `kobject`s
* **Classes and Buses**: Represented using `kobject` and `kset`
* **Custom Drivers**: You can define your own `kobject` to expose driver state or controls


## Reference Counting

`kobject`s include a **reference counter**, so they aren't destroyed while still in use. They support automatic cleanup using release callbacks.


## In Summary

| Term      | Meaning                                                        |
| --------- | -------------------------------------------------------------- |
| `kobject` | Core kernel object for representing structured entities        |
| `sysfs`   | Virtual FS used to expose kernel objects to userspace          |
| Relation  | A `kobject` is what makes a kernel structure show up in `/sys` |

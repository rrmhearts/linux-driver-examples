# Sample Drivers

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

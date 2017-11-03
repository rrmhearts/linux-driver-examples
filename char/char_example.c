#include <linux/init.h>   // module_init and module_exit
#include <linux/module.h> // THIS_MODULE, kernel versioning info
#include <linux/fs.h>     // register_chrdev and unregister_chrdev
#include <linux/kernel.h> // printk
#include <linux/cdev.h>   // character device stuff
#include <linux/errno.h>  // error codes
#include <asm/uaccess.h>  // copy_to_user()


static const char    g_s_Hello_World_string[] = "Hello world from kernel mode!\n\0";
static const ssize_t g_s_Hello_World_size = sizeof(g_s_Hello_World_string);
static ssize_t device_file_read(
                        struct file *file_ptr
                       , char __user *user_buffer
                       , size_t count
                       , loff_t *position)
{
    printk( KERN_NOTICE "Sample-driver: Device file is read at offset = %i, read bytes count = %u"
                , (int)*position
                , (unsigned int)count );
    /* If position is behind the end of a file we have nothing to read */
    if( *position >= g_s_Hello_World_size )
        return 0;
    /* If a user tries to read more than we have, read only as many bytes as we have */
    if( *position + count > g_s_Hello_World_size )
        count = g_s_Hello_World_size - *position;
    if( copy_to_user(user_buffer, g_s_Hello_World_string + *position, count) != 0 )
        return -EFAULT;    
    /* Move reading position */
    *position += count;
    return count;
}

static struct file_operations sample_driver_fops = 
{
    .owner   = THIS_MODULE,
    .read    = device_file_read,
};

static int device_file_major_number = 0;
static const char device_name[] = "Sample-driver";
static int register_device(void)
{
        int result = 0;
        printk( KERN_NOTICE "Sample-driver: register_device() is called." );
        result = register_chrdev( 0, device_name, &sample_driver_fops );
        if( result < 0 )
        {
            printk( KERN_WARNING "Sample-driver:  can\'t register character device with errorcode = %i", result );
            return result;
        }
        device_file_major_number = result;
        printk( KERN_NOTICE "Sample-driver: registered character device with major number = %i and minor numbers 0...255"
             , device_file_major_number );
        return 0;
}

void unregister_device(void)
{
	printk( KERN_NOTICE "Sample-driver: unregister_device() is called" );
	if(device_file_major_number != 0)
	{
		unregister_chrdev(device_file_major_number, device_name);
	}
}

static int my_init(void)
{
	register_device();
	return  0;
}
 
static void my_exit(void)
{
	unregister_device();
	return;
}
 
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ryan McCoppin");


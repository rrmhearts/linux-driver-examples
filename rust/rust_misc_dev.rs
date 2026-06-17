//! A simple misc device driver written in Rust.
//!
//! This driver creates a /dev/rust_misc_dev node. When read, 
//! it outputs a simple greeting from the kernel.

use kernel::prelude::*;
use kernel::miscdev;
use kernel::file_operations::{File, Operations};
use kernel::uaccess::{UserSliceReader, UserSliceWriter};

// 1. Module Definition
// Replaces MODULE_LICENSE, MODULE_AUTHOR, module_init, and module_exit.
module! {
    type: RustMiscDevModule,
    name: "rust_misc_dev",
    author: "Your Name",
    description: "A simple misc device written in Rust",
    license: "GPL",
}

// 2. The Device State
// This struct holds the state of our module. In this case, it just holds 
// the registration object. When this struct is dropped (on module unload), 
// the misc device is automatically deregistered.
struct RustMiscDevModule {
    _miscdev: miscdev::Registration<RustMiscDev>,
}

// A marker struct for our file operations
struct RustMiscDev;

// 3. File Operations
// The #[vtable] macro safely exposes these methods as C function pointers
// to the kernel's VFS layer.
#[vtable]
impl Operations for RustMiscDev {
    type OpenData = ();
    type Data = ();

    fn open(_context: &Self::OpenData, _file: &File) -> Result<Self::Data> {
        pr_info!("rust_misc_dev: Device opened\n");
        Ok(())
    }

    fn read(
        _data: &Self::Data,
        _file: &File,
        mut writer: impl UserSliceWriter,
        _offset: u64,
    ) -> Result<usize> {
        let message = b"Hello from the Rust Linux Kernel Driver!\n";
        
        // write_slice handles the copy_to_user logic safely
        writer.write_slice(message)?;
        
        pr_info!("rust_misc_dev: Data read by user\n");
        Ok(message.len())
    }
}

// 4. Module Initialization
// Implementing the Module trait is equivalent to module_init().
// The Drop trait (implicitly handled here by the destruction of Registration)
// replaces module_exit().
impl kernel::Module for RustMiscDevModule {
    fn init(name: &'static CStr, _module: &'static kernel::ThisModule) -> Result<Self> {
        pr_info!("rust_misc_dev: Initializing module\n");

        // Register the misc device. 
        // The abstraction automatically populates the underlying C structures.
        let reg = miscdev::Registration::new_pinned(fmt!("{name}"), ())?;

        Ok(RustMiscDevModule {
            _miscdev: reg,
        })
    }
}

# Rust for Linux Kernel Device Drivers

## Introduction

As of Linux 6.1, Rust has been officially introduced as a secondary language for kernel development. While C remains the primary language, the "Rust for Linux" (RfL) project provides a framework for writing kernel modules—including device drivers—in Rust.

This guide bridges the gap between traditional C-based driver development (character, I2C, platform, sysfs, misc) and the new Rust abstractions.

## Why Rust in the Kernel?

The primary difference is **compile-time guarantees**. Rust aims to eliminate entire classes of undefined behavior at compile time without introducing runtime overhead.

* **Memory Safety:** Rust’s ownership and borrowing models prevent use-after-free, double-free, and null pointer dereferences.
* **Concurrency Safety:** Data races are caught at compile time. If a struct requires a lock (like a mutex or spinlock) to be accessed safely in a multithreaded environment (like an interrupt handler or concurrent `read`/`write` syscalls), the Rust compiler enforces that the lock is acquired before the data is touched.
* **Safe Abstractions:** RfL provides "safe wrappers" around the kernel's underlying C APIs. Once you are within the safe Rust abstraction, you cannot misuse the C API.

## Prerequisites & Setup

Unlike C, which only requires `gcc`/`clang` and `make`, Rust kernel modules require a specific toolchain:

1. **Rust Toolchain:** `rustc` and `cargo` (usually managed via `rustup`).
2. **Bindgen:** A tool that generates Rust bindings from the kernel's C headers.
3. **Kernel Config:** The kernel must be compiled with `CONFIG_RUST=y`.

## Core Concepts: Rust vs. C

| Concept | Traditional C | Rust for Linux |
| --- | --- | --- |
| **Module Definition** | `module_init()`, `module_exit()`, `MODULE_LICENSE()` | The `module!` macro. |
| **Initialization** | Returns `int` (0 or `-ERRNO`). | Returns `Result<T, Error>`. |
| **Memory Allocation** | `kmalloc`, `kfree` | Smart pointers like `Box`, `Arc`, and standard collections (using kernel allocators). |
| **Vtables (File Ops)** | `struct file_operations` with function pointers. | Traits mapped with the `#[vtable]` macro. |
| **Printing** | `printk(KERN_INFO ...)`, `pr_info()` | `pr_info!()`, `pr_err!()` macros (supports Rust formatting). |

## Example: A Rust `misc` Device Driver

The the example, please view the [rust_misc_dev.rs](./rust_misc_dev.rs) implementation. Notice how the `module!` macro replaces the standard C boilerplate, and the `#[vtable]` macro safely maps Rust functions to the C `file_operations` struct.

### Key Takeaways from the Example:

1. **No `copy_to_user`:** Instead of manually calling `copy_to_user` and checking the return value, the Rust driver uses `UserSliceWriter::write_slice()`. If the user-space pointer is invalid, the Rust abstraction safely catches it and returns an `EFAULT` error via the `Result` type.
2. **Automatic Cleanup (RAII):** Notice there is no explicit `misc_deregister` or `module_exit` function. In Rust, when `RustMiscDevModule` goes out of scope (when the module is unloaded), its `Drop` implementation is called. The `miscdev::Registration` object's drop handler automatically calls the C `misc_deregister` function, ensuring you never forget to clean up resources.
3. **Strict Error Handling:** The `Result<T>` return type forces the developer to handle errors. You cannot accidentally ignore a failed initialization.

> **Note on API Stability:** The Rust-for-Linux abstractions are actively evolving. Functions like `UserSliceWriter` or `miscdev::Registration` may change syntax depending on the exact kernel version (6.1 vs 6.8+). Always refer to the `rust/kernel/` directory in your specific kernel tree for the exact trait definitions.
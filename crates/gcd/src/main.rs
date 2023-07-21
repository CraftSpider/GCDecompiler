use std::fs;
use std::path::PathBuf;
use clap::{Args, Parser, Subcommand, ValueEnum};
use tracing_subscriber::filter::LevelFilter;
use tracing_subscriber::FmtSubscriber;
use gcd_tpl::TplKind;

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Cli {
    #[command(flatten)]
    log: LogLevel,
    #[command(subcommand)]
    subcommand: Command,
}

#[derive(Subcommand)]
enum Command {
    /// Functionality for reading/writing TPL files
    Tpl {
        /// Whether to extract or compile images
        #[arg(value_enum)]
        mode: TplMode,
        #[command(flatten)]
        inout: InOut,
    }
}

#[derive(Args)]
#[group(required = false, multiple = false)]
pub struct LogLevel {
    /// Increase log verbosity. Declare multiple times to increase verbosity further
    #[arg(short, action = clap::ArgAction::Count, value_parser = clap::value_parser!(u8).range(..3))]
    v: u8,
    /// Decrease log verbosity. Declare multiple times to decrease verbosity further
    #[arg(short, action = clap::ArgAction::Count, value_parser = clap::value_parser!(u8).range(..3))]
    q: u8,
}

impl LogLevel {
    fn level(&self) -> LevelFilter {
        match (self.v, self.q) {
            (0, 3) => LevelFilter::OFF,
            (0, 2) => LevelFilter::ERROR,
            (0, 1) => LevelFilter::WARN,
            (0, 0) => LevelFilter::INFO,
            (1, 0) => LevelFilter::DEBUG,
            (2, 0) => LevelFilter::TRACE,
            _ => unreachable!(),
        }
    }
}

#[derive(Args)]
pub struct InOut {
    #[arg(required = true)]
    input: PathBuf,
    output: Option<PathBuf>,
}

#[derive(Copy, Clone, PartialEq, Eq, ValueEnum)]
enum TplMode {
    /// Unpack a `.tpl` into multiple `.png` files
    Extract,
    /// Pack several `.png` files into a `.tpl`
    Build,
}

fn main() {
    let cli = Cli::parse();
    
    let lvl_filt = cli.log.level();
    
    let subscriber = FmtSubscriber::builder()
        .with_max_level(lvl_filt)
        .finish();
    
    tracing::subscriber::set_global_default(subscriber)
        .expect("Setting tracing default failed");
    
    match cli.subcommand {
        Command::Tpl { mode, inout } => {
            do_tpl(mode, inout)
        }
    }
}

fn do_tpl(mode: TplMode, inout: InOut) {
    match mode {
        TplMode::Extract => {
            let output = inout.output.unwrap_or(PathBuf::from("out/"));
            fs::create_dir_all(&output)
                .unwrap();
            let tpl = gcd_tpl::Tpl::read(inout.input, TplKind::Gc)
                .unwrap();
            for (i, mipmap) in tpl.mipmaps().iter().enumerate() {
                for (j, image) in mipmap.images().iter().enumerate() {
                    println!("Image {}_{} Format: {:?}", i, j, image.format());
                    image.write_png(output.join(format!("{}_{}.png", i, j)))
                        .unwrap();
                }
            }
        }
        TplMode::Build => {
            todo!()
        }
    }
}

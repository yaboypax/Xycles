use std::env;
use std::fs;
use std::path::PathBuf;

fn main() {
    cxx_build::bridge("src/lib.rs")
        .compile("rust_part");

    println!("cargo:rerun-if-changed=src/lib.rs");

    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());

    let generated_header = out_dir
        .join("cxxbridge")
        .join("include")
        .join("rust_part")
        .join("src")
        .join("lib.rs.h");

    let stable_header = PathBuf::from("rust_part.h");
    fs::copy(&generated_header, &stable_header).unwrap();

}
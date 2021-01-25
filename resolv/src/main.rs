use std::fs::File;
use std::io::prelude::*;

extern crate resolv_conf;

const RESOLV_CONF: &str = "/etc/resolv.conf";

fn main() {
    let mut contents = Vec::new();
    let mut file = File::open(RESOLV_CONF).unwrap();
    file.read_to_end(&mut contents).unwrap();
    let cfg = resolv_conf::Config::parse(&contents).unwrap();
    println!("---- Config -----\n{:#?}\n", cfg);
}

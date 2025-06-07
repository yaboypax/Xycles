use std::fs::File;
use std::io::BufReader;
use std::sync::Mutex;
use rodio::{Decoder, OutputStream, Sink, source::Source};
mod load_file;
#[cxx::bridge]
mod ffi {
    #[namespace = "rust_part"]
    extern "Rust" {
        fn set_gain(gain: f32);
        fn set_speed(pitch: f32);
        fn get_audio(_path: &str);
    }
}

static GAIN_STATE: Mutex<f32> = Mutex::new(1.0);
static SPEED_STATE: Mutex<f32> = Mutex::new(1.0);

pub fn set_gain(g: f32) {
    let mut gain = GAIN_STATE.lock().unwrap();
    *gain = g;
}
pub fn set_speed(s: f32) {
    let mut speed = SPEED_STATE.lock().unwrap();
    *speed = s;
}
pub fn get_audio(_path: &str) {
    let gain = GAIN_STATE.lock().unwrap();
    let speed = SPEED_STATE.lock().unwrap();

    let (_stream, stream_handle) = OutputStream::try_default().unwrap();
    let sink = Sink::try_new(&stream_handle).unwrap();
    let file = BufReader::new(File::open(_path).unwrap());
    let source = Decoder::new(file).unwrap();

    sink.append(source);
    sink.set_volume(*gain);
    sink.set_speed(*speed);
        
    std::thread::sleep(std::time::Duration::from_secs(5));
}
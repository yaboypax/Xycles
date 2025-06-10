use std::fs::File;
use std::io::BufReader;
use std::sync::Mutex;
use rodio::{Decoder, OutputStream, Sink, source::Source};
mod load_file;
#[cxx::bridge]
mod ffi {
    #[namespace = "rust_part"]
    extern "Rust" {
        type Engine;
        fn new_engine() -> Box<Engine>;
        fn set_gain(self: &mut Engine, gain: f32);
        fn set_speed(self: &mut Engine, speed: f32);
        fn load_audio(self: &mut Engine, path: &str);
    }
}

pub fn new_engine() -> Box<Engine> { Box::new(Engine::new()) }
pub struct Engine {
    gain: f32,
    speed: f32,
    audio_sink: Sink
}
impl Engine {
    pub fn new() -> Self {
        let (_stream, handle) = OutputStream::try_default().unwrap();
        let sink = Sink::try_new(&handle).unwrap();
        Engine { gain: 1.0, speed: 1.0, audio_sink: sink }
    }
    pub fn set_gain(&mut self, gain: f32) {
        self.gain = gain;
        self.audio_sink.set_volume(gain);
    }
    pub fn set_speed(&mut self, speed: f32) {
        self.speed = speed;
        self.audio_sink.set_speed(speed);
    }
    pub fn load_audio(&mut self, path: &str) {
        let (_stream, handle) = OutputStream::try_default().unwrap();
        let new_sink = Sink::try_new(&handle).unwrap();
        let file = BufReader::new(File::open(path).unwrap());
        let source = Decoder::new(file).unwrap();
        new_sink.append(source);
        self.audio_sink = new_sink;
    }
}




// pub fn get_next_samples() -> (f32, f32)
// {
//     return ()
// }
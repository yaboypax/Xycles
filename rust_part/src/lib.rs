use std::fs::File;
use std::io::BufReader;
use rodio::{Decoder, OutputStream, source::Source};
mod load_file;
#[cxx::bridge]
mod ffi {
    #[namespace = "rust_part"]
    extern "Rust" {
        fn process_channel_gain(channel_data: &mut [f32], gain: f32);
        fn get_audio(_path: &str);
    }
}
#[no_mangle]
pub fn process_channel_gain(channel_data: &mut [f32], gain: f32) {
    channel_data.iter_mut().for_each(|s| *s *= gain);
}

#[no_mangle]
pub fn get_audio(_path: &str) {
    // Get an output stream handle to the default physical sound device.
    // Note that no sound will be played if _stream is dropped
    let (_stream, stream_handle) = OutputStream::try_default().unwrap();
    // Load a sound from a file, using a path relative to Cargo.toml
    let file = BufReader::new(File::open(_path).unwrap());
    // Decode that sound file into a source
    let source = Decoder::new(file).unwrap();
    // Play the sound directly on the device
    let _result = stream_handle.play_raw(source.convert_samples());

    // The sound plays in a separate audio thread,
    // so we need to keep the main thread alive while it's playing.
    std::thread::sleep(std::time::Duration::from_secs(5));
}
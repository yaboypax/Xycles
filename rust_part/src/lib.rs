#[cxx::bridge]
mod ffi {
    #[namespace = "rust_part"]
    extern "Rust" {
        fn process_channel_gain(channel_data: &mut [f32], gain: f32);
    }
}
#[no_mangle]
pub fn process_channel_gain(channel_data: &mut [f32], gain: f32) {
    channel_data.iter_mut().for_each(|s| *s *= gain);
}
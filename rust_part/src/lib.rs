mod track;
mod engine;

pub use engine::{Engine, EngineEvent};

pub fn new_engine() -> Box<Engine> { Box::new(Engine::new()) }

#[cxx::bridge]
mod ffi {
    #[namespace = "rust_part"]
    extern "Rust" {
        type Engine;
        type EngineEvent;
        
        fn new_engine() -> Box<Engine>;
        fn process_block(self: &mut Engine, buffer: &mut Vec<f32>);
        fn load_audio(self: &mut Engine, path: &str);
        
        fn play(self: &mut Engine);
        fn pause(self: &mut Engine);
        fn stop(self: &mut Engine);
        
        fn set_gain(self: &mut Engine, gain: f32);
        fn set_speed(self: &mut Engine, speed: f32);

        fn set_start(self: &mut Engine, start: f32);
        fn set_end(self: &mut Engine, end: f32);
        
        fn set_grain_speed(self: &mut Engine, speed: f32);
        fn set_grain_length(self: &mut Engine, length: f32);
        fn set_grain_overlap(self: &mut Engine, overlap: f32);
        fn set_grain_count(self: &mut Engine, count: f32);
        fn set_grain_spread(self: &mut Engine, spread: f32);
        
        fn get_playhead(self: &Engine) -> f32;
    }
}
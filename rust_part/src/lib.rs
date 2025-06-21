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
        
        fn load_audio(self: &mut Engine, path: &str);
        fn play(self: &mut Engine);
        fn pause(self: &mut Engine);
        fn stop(self: &mut Engine);
        fn set_gain(self: &mut Engine, gain: f32);
        fn set_speed(self: &mut Engine, speed: f32);
    }
}
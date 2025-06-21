

    // use std::fs::File;
    // use std::io::BufReader;
    // use rodio::{Decoder, OutputStream, Sink};
    // 
    // pub struct Engine {
    //     gain: f32,
    //     speed: f32,
    //     audio_sink: Sink
    // }
    // impl Engine {
    //     pub fn new() -> Self {
    //         let (_stream, handle) = OutputStream::try_default().unwrap();
    //         let sink = Sink::try_new(&handle).unwrap();
    //         Engine { gain: 1.0, speed: 1.0, audio_sink: sink }
    //     }
    //     pub fn set_gain(&mut self, gain: f32) {
    //         self.gain = gain;
    //         self.audio_sink.set_volume(gain);
    //     }
    //     pub fn set_speed(&mut self, speed: f32) {
    //         self.speed = speed;
    //         self.audio_sink.set_speed(speed);
    //     }
    //     pub fn load_audio(&mut self, path: &str) {
    //         let (_stream, handle) = OutputStream::try_default().unwrap();
    //         let new_sink = Sink::try_new(&handle).unwrap();
    //         let file = BufReader::new(File::open(path).unwrap());
    //         let source = Decoder::new(file).unwrap();
    //         new_sink.append(source);
    //         self.audio_sink = new_sink;
    //     }
    // }



    use rodio::{Decoder, OutputStream, Sink, source::Source};
    use std::{fs::File, io::BufReader, mem};
    
    pub enum EngineState {
        Idle,
        Playing {
            stream: OutputStream,
            sink: Sink,
            gain: f32,
            speed: f32,
        },
        Paused {
            stream: OutputStream,
            sink: Sink,
            gain: f32,
            speed: f32,
        },
    }
    #[derive(Debug)]
    pub enum EngineEvent {
        Load(String),   
        Play,           
        Pause,          
        Stop,          
        SetGain(f32),
        SetSpeed(f32),
    }

    pub struct Engine {
        state: EngineState,
    }

    impl Engine {

        // FFI Bridge
        pub fn load_audio(&mut self, path: &str)   { self.transition(EngineEvent::Load(path.into())) }
        pub fn play(&mut self)                     { self.transition(EngineEvent::Play) }
        pub fn pause(&mut self)                    { self.transition(EngineEvent::Pause) }
        pub fn stop(&mut self)                     { self.transition(EngineEvent::Stop) }
        pub fn set_gain(&mut self, g: f32)         { self.transition(EngineEvent::SetGain(g)) }
        pub fn set_speed(&mut self, s: f32)        { self.transition(EngineEvent::SetSpeed(s)) }
        
        
        pub fn new() -> Self {
            Engine { state: EngineState::Idle }
        }
        
        pub fn transition(&mut self, event: EngineEvent) {
            let old = mem::replace(&mut self.state, EngineState::Idle);
            self.state = match (old, event) {

                // ─── Idle ───
                (EngineState::Idle, EngineEvent::Load(path)) => {
                    let (stream, handle) = OutputStream::try_default().unwrap();
                    let mut sink = Sink::try_new(&handle).unwrap();
                    let file   = BufReader::new(File::open(&path).unwrap());
                    let source = Decoder::new(file).unwrap();
                    sink.append(source);

                    EngineState::Playing { stream, sink, gain: 1.0, speed: 1.0 }
                }
                (EngineState::Idle, EngineEvent::Play) => {
                    dbg!("play");
                    EngineState::Idle
                }
                (EngineState::Idle, EngineEvent::Pause)
                | (EngineState::Idle, EngineEvent::Stop)
                => EngineState::Idle,

                // ─── Playing ───
                (EngineState::Playing { stream, sink, gain, speed }, EngineEvent::Pause) => {
                    sink.pause();
                    EngineState::Paused { stream, sink, gain, speed }
                }
                (EngineState::Playing { stream, sink, gain, speed }, EngineEvent::Stop) => {
                    sink.stop();
                    EngineState::Idle
                }
                (EngineState::Playing { stream, sink, mut gain, speed }, EngineEvent::SetGain(g)) => {
                    gain = g;
                    sink.set_volume(g);
                    EngineState::Playing { stream, sink, gain, speed }
                }
                (EngineState::Playing { stream, sink, gain, mut speed }, EngineEvent::SetSpeed(s)) => {
                    speed = s;
                    sink.set_speed(s);
                    EngineState::Playing { stream, sink, gain, speed }
                }
                (EngineState::Playing { stream, sink, gain, speed }, EngineEvent::Play) => {
                    EngineState::Playing { stream, sink, gain, speed }
                }
                
                (EngineState::Paused { stream, sink, gain, speed }, EngineEvent::Play) => {
                    sink.play();
                    EngineState::Playing { stream, sink, gain, speed }
                }
                
                (EngineState::Paused { stream, sink, gain, speed }, EngineEvent::Stop) => {
                    sink.stop();
                    EngineState::Idle
                }
                (EngineState::Paused { stream, sink, mut gain, speed }, EngineEvent::SetGain(g)) => {
                    gain = g;
                    EngineState::Paused { stream, sink, gain, speed }
                }
                
                (EngineState::Paused { stream, sink, gain, mut speed }, EngineEvent::SetSpeed(s)) => {
                    speed = s;
                    EngineState::Paused { stream, sink, gain, speed }
                }
                (EngineState::Paused { stream, sink, gain, speed }, EngineEvent::Pause) => {
                    EngineState::Paused { stream, sink, gain, speed }
                }

                // no state change
                (old_state, _) => old_state,
            };
        }
    }

    use dasp::{Frame};
    use rodio::{Decoder, OutputStream, Sink, source::Source, buffer::SamplesBuffer};
    use std::{fs::File, io::BufReader, mem};
    
    pub enum EngineState {
        Idle,
        Ready {
            samples: SamplesBuffer<f32>,
            position: usize,
            gain: f32,
            speed: f32,
        },
        Playing {
            samples: SamplesBuffer<f32>,
            position: usize,
            gain: f32,
            speed: f32, 
        },
        Paused {
            samples: SamplesBuffer<f32>,
            position: usize,
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
        
        pub fn process_block(&mut self, buffer: &mut Vec<f32>)
        {
            for sample in buffer.iter_mut() {*sample = 0.0}; //mute
            
            match &mut self.state {
                EngineState::Idle => {
                    
                }
                
                EngineState::Ready {samples, position, gain, speed } => {

                }

                EngineState::Playing {samples, position, gain, speed } => {
                    for outputSample in buffer.iter_mut() {
                        let s = samples.next().unwrap_or(0.0); 
                        *outputSample = s * *gain;
                    }
                }

                EngineState::Paused {samples, position, gain, speed } => {

                }
            }
            
        }
        
        
        pub fn load_path(&mut self, path: String) -> EngineState
        {
            let file   = BufReader::new(File::open(&path).unwrap());
            let source = Decoder::new(file).unwrap();
            let buffer= SamplesBuffer::new(source.channels(), source.sample_rate(), source.convert_samples().collect::<Vec<f32>>());
            EngineState::Ready { samples: buffer, position: 0, gain: 1.0, speed: 1.0 }
        }
        

        
        pub fn transition(&mut self, event: EngineEvent) {
            let old = mem::replace(&mut self.state, EngineState::Idle);
            self.state = match (old, event) {

                // ─── Idle ───
                (EngineState::Idle, EngineEvent::Load(path)) => {
                    self.load_path(path)
                }
                (EngineState::Ready { samples, position, gain, speed }, EngineEvent::Play) => {
                    EngineState::Playing { samples, position, gain, speed }
                }

                // ─── Playing ───
                (EngineState::Playing { samples, position, gain, speed }, EngineEvent::Pause) => {
                    EngineState::Paused { samples, position, gain, speed }
                }
                (EngineState::Playing { samples, position, gain, speed }, EngineEvent::Stop) => {
                    EngineState::Ready { samples, position, gain, speed }
                }
                (EngineState::Playing { samples, position, mut gain, speed }, EngineEvent::SetGain(g)) => {
                    gain = g;
                    EngineState::Playing { samples, position, gain, speed }
                }
                (EngineState::Playing { samples, position, gain, mut speed }, EngineEvent::SetSpeed(s)) => {
                    speed = s;
                    EngineState::Playing { samples, position, gain, speed }
                }
                (EngineState::Playing { samples, position, gain, speed }, EngineEvent::Play) => {
                    EngineState::Playing { samples, position, gain, speed }
                }
                
                (EngineState::Paused { samples, position, gain, speed }, EngineEvent::Play) => {
                    EngineState::Playing { samples, position, gain, speed }
                }
                
                (EngineState::Paused { samples, position, gain, speed }, EngineEvent::Stop) => {
                    EngineState::Ready { samples, position, gain, speed }
                }
                (EngineState::Paused { samples, position, mut gain, speed }, EngineEvent::SetGain(g)) => {
                    gain = g;
                    EngineState::Paused { samples, position, gain, speed }
                }
                
                (EngineState::Paused { samples, position, gain, mut speed }, EngineEvent::SetSpeed(s)) => {
                    speed = s;
                    EngineState::Paused { samples, position, gain, speed }
                }

                // no state change
                (old_state, _) => old_state,
            };
        }
    }
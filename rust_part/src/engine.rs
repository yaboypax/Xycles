
    use dasp::{Frame};
    use rodio::{Decoder, OutputStream, Sink, source::Source, buffer::SamplesBuffer};
    use std::{fs::File, io::BufReader, mem};
    
    pub enum EngineState {
        Idle,
        Ready {
            samples:    Vec<f32>,
            position:   f32,
            start:      usize, 
            end:        usize,
            channels:   usize,
            gain:       f32,
            speed:      f32,
        },
        Playing {
            samples:    Vec<f32>,
            position:   f32,
            start:      usize,
            end:        usize,
            channels:   usize,
            gain:       f32,
            speed:      f32,
        },
        Paused {
            samples:    Vec<f32>,
            position:   f32,
            start:      usize,
            end:        usize,
            channels:   usize,
            gain:       f32,
            speed:      f32,
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
        
        pub fn fill_silence(buffer: &mut Vec<f32>) {for sample in buffer.iter_mut() {*sample = 0.0}; }

        pub fn process_block(&mut self, buffer: &mut Vec<f32>)
        {
            match &mut self.state {
                EngineState::Idle => {
                    Self::fill_silence(buffer);
                }

                EngineState::Ready { samples, position, start, end, channels, gain, speed } => {
                    Self::fill_silence(buffer);
                }

                EngineState::Playing { samples, position, start, end, channels, gain, speed } => {
                    let total_frames = samples.len() / *channels;
                    let block = buffer.len() / *channels;

                    for frame in 0..block {
                        if *position < *start as f32 {
                            *position = (*end - 1) as f32 + (*position - *start as f32);
                        } else if *position >= *end as f32 {
                            *position = *start as f32 + (*position - *end as f32);
                        }

                        let index = *position as usize;
                        let base = index * *channels;
                        let next = ((index + 1) % total_frames) * *channels;

                        for ch in 0..*channels {
                            let s0 = samples[base + ch];
                            let s1 = samples[next + ch];
                            let s = s0 + (s1 - s0) * position.fract();
                            buffer[frame * *channels + ch] = s * *gain;
                        }

                        *position += *speed;
                    }
                }
            

            EngineState::Paused { samples, position, start, end, channels, gain, speed } => {
                    Self::fill_silence(buffer);
                }
            }
            
        }
        
        
        pub fn load_path(&mut self, path: String) -> EngineState
        {
            let file   = BufReader::new(File::open(&path).unwrap());
            let source = Decoder::new(file).unwrap();
            let channels = source.channels() as usize;
            let samples: Vec<f32> = source.convert_samples().collect();
            let end = samples.len() / channels;
            EngineState::Ready { samples, position: 0.0, start: 0, end, channels, gain: 1.0, speed: 1.0 }
        }
        

        
        pub fn transition(&mut self, event: EngineEvent) {
            let old = mem::replace(&mut self.state, EngineState::Idle);
            self.state = match (old, event) {

                // ─── Idle ───
                (EngineState::Idle, EngineEvent::Load(path)) => {
                    self.load_path(path)
                }
                (EngineState::Ready { samples, position, start, end, channels, gain, speed }, EngineEvent::Play) => {
                    EngineState::Playing { samples, position, start, end, channels, gain, speed }
                }

                // ─── Playing ───
                (EngineState::Playing { samples, position, start, end, channels, gain, speed }, EngineEvent::Pause) => {
                    EngineState::Paused { samples, position, start, end, channels, gain, speed }
                }
                (EngineState::Playing { samples, position, start, end, channels, gain, speed }, EngineEvent::Stop) => {
                    EngineState::Ready { samples, position, start, end, channels, gain, speed }
                }
                (EngineState::Playing { samples, position, start, end, channels, mut gain, speed }, EngineEvent::SetGain(g)) => {
                    gain = g;
                    EngineState::Playing { samples, position, start, end, channels, gain, speed }
                }
                (EngineState::Playing { samples, position, start, end, channels, gain, mut speed }, EngineEvent::SetSpeed(s)) => {
                    speed = s;
                    EngineState::Playing { samples, position, start, end, channels, gain, speed }
                }
                (EngineState::Playing { samples, position, start, end, channels, gain, speed }, EngineEvent::Play) => {
                    EngineState::Playing { samples, position, start, end, channels, gain, speed }
                }
                
                (EngineState::Paused { samples, position, start, end, channels, gain, speed }, EngineEvent::Play) => {
                    EngineState::Playing { samples, position, start, end, channels, gain, speed }
                }
                
                (EngineState::Paused { samples, position, start, end, channels, gain, speed }, EngineEvent::Stop) => {
                    EngineState::Ready { samples, position, start, end, channels, gain, speed }
                }
                (EngineState::Paused { samples, position, start, end, channels, mut gain, speed }, EngineEvent::SetGain(g)) => {
                    gain = g;
                    EngineState::Paused { samples, position, start, end, channels, gain, speed }
                }
                
                (EngineState::Paused { samples, position, start, end, channels, gain, mut speed }, EngineEvent::SetSpeed(s)) => {
                    speed = s;
                    EngineState::Paused { samples, position, start, end, channels, gain, speed }
                }

                // no state change
                (old_state, _) => old_state,
            };
        }
    }
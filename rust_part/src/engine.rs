
    use dasp::{Frame};
    use rodio::{Decoder, OutputStream, Sink, source::Source, buffer::SamplesBuffer};
    use std::{fs::File, io::BufReader, mem};
    pub struct Track {
        samples:    Vec<f32>,
        position:   f32,
        start:      usize,
        end:        usize,
        channels:   usize,
        gain:       f32,
        speed:      f32,
    }
    pub enum EngineState {
        Idle,
        Ready (Track),
        Playing (Track),
        Paused (Track),
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

                EngineState::Ready (_track) => {
                    Self::fill_silence(buffer);
                }

                EngineState::Playing (track) => {
                    let total_frames = track.samples.len() / track.channels;
                    let block = buffer.len() / track.channels;

                    for frame in 0..block {
                        if track.position < track.start as f32 {
                            track.position = (track.end - 1) as f32 + (track.position - track.start as f32);
                        } else if track.position >= track.end as f32 {
                            track.position = track.start as f32 + (track.position - track.end as f32);
                        }

                        let index = track.position as usize;
                        let base = index * track.channels;
                        let next = ((index + 1) % total_frames) * track.channels;

                        for ch in 0..track.channels {
                            let s0 = track.samples[base + ch];
                            let s1 = track.samples[next + ch];
                            let s = s0 + (s1 - s0) * track.position.fract();
                            buffer[frame * track.channels + ch] = s * track.gain;
                        }

                        track.position += track.speed;
                    }
                }
            

            EngineState::Paused (_track) => {
                    Self::fill_silence(buffer);
                }
            };
            
        }
        
        
        pub fn load_path(&mut self, path: String) -> EngineState
        {
            let file   = BufReader::new(File::open(&path).unwrap());
            let source = Decoder::new(file).unwrap();
            let channels = source.channels() as usize;
            let samples: Vec<f32> = source.convert_samples().collect();
            let end = samples.len() / channels;
            let track = Track{ samples, position: 0.0, start: 0, end, channels, gain: 1.0, speed: 1.0 };
            EngineState::Ready(track)
        }
        

        
        pub fn transition(&mut self, event: EngineEvent) {
            let old = mem::replace(&mut self.state, EngineState::Idle);
            self.state = match (old, event) {

                // ─── Idle ───
                (EngineState::Idle, EngineEvent::Load(path)) => {
                    self.load_path(path)
                }
                (EngineState::Ready (track), EngineEvent::Play) => {
                    EngineState::Playing (track)
                }

                // ─── Playing ───
                (EngineState::Playing (track), EngineEvent::Pause) => {
                    EngineState::Paused (track)
                }
                (EngineState::Playing (track), EngineEvent::Stop) => {
                    EngineState::Ready (track)
                }
                (EngineState::Playing (track), EngineEvent::SetGain(g)) => {
                    let mut t = track;
                    t.gain = g;
                    EngineState::Playing (t)
                }
                (EngineState::Playing (track), EngineEvent::SetSpeed(s)) => {
                    let mut t = track;
                    t.speed = s;
                    EngineState::Playing (t)
                }
                (EngineState::Playing (track), EngineEvent::Play) => {
                    EngineState::Playing (track)
                }
                
                (EngineState::Paused (track), EngineEvent::Play) => {
                    EngineState::Playing (track)
                }
                
                (EngineState::Paused (track), EngineEvent::Stop) => {
                    EngineState::Ready (track)
                }
                (EngineState::Paused (track), EngineEvent::SetGain(g)) => {
                    let mut t = track;
                    t.gain = g;
                    EngineState::Playing (t)
                }
                
                (EngineState::Paused (track), EngineEvent::SetSpeed(s)) => {
                    let mut t = track;
                    t.speed = s;
                    EngineState::Playing (t)
                }

                // no state change
                (old_state, _) => old_state,
            };
        }
    }
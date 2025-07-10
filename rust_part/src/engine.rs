
    use std::sync::atomic::{AtomicU32, Ordering};
    use rodio::{Decoder, source::Source};
    use std::{fs::File, io::BufReader, mem};
    use crossbeam_queue::ArrayQueue;
    use std::sync::Arc;
    pub struct Track {
        samples:    Vec<f32>,
        position:   f32,
        start:      usize,
        end:        usize,
        channels:   usize,
        gain:       AtomicU32,
        speed:      AtomicU32,
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
        SetStart(f32),
        SetEnd(f32),
    }

    pub struct Engine {
        state: EngineState,
        commands: Arc<ArrayQueue<EngineEvent>>,
    }

    impl Engine {
        fn push_event(&self, event: EngineEvent) {
            let _ = self.commands.push(event);
        }

        fn apply_pending(&mut self) {
            while let Some(event) = self.commands.pop() {
                self.transition(event);
            }
        }

        // FFI Bridge
        pub fn load_audio(&mut self, path: &str)   { self.push_event(EngineEvent::Load(path.into())) }
        pub fn play(&mut self)                     { self.push_event(EngineEvent::Play) }
        pub fn pause(&mut self)                    { self.push_event(EngineEvent::Pause) }
        pub fn stop(&mut self)                     { self.push_event(EngineEvent::Stop) }
        pub fn set_gain(&mut self, g: f32)         { self.push_event(EngineEvent::SetGain(g)) }
        pub fn set_speed(&mut self, s: f32)        { self.push_event(EngineEvent::SetSpeed(s)) }
        pub fn set_start(&mut self, start: f32)    { self.push_event(EngineEvent::SetStart(start)) }
        pub fn set_end(&mut self, end: f32)        { self.push_event(EngineEvent::SetEnd(end)) }
        
        pub fn get_playhead(&self) -> f32
        {
            match &self.state {
                EngineState::Playing(track) | EngineState::Paused(track)
                | EngineState::Ready(track) => {
                    if track.start < track.end {
                        let loop_length = (track.end - track.start) as f32;
                        track.position.clamp(0.0, loop_length) / loop_length
                    } else {
                         0.0
                    }
                }
                EngineState::Idle => 0.0,
            }
        }

        pub fn new() -> Self {
            Engine { state: EngineState::Idle, commands: Arc::new(ArrayQueue::new(32))}
        }
        
        pub fn fill_silence(buffer: &mut Vec<f32>) {for sample in buffer.iter_mut() {*sample = 0.0}; }
        

        pub fn process_block(&mut self, buffer: &mut Vec<f32>)
        {
            self.apply_pending();
            match &mut self.state {
                EngineState::Playing (track) => {

                    let gain  = f32::from_bits(track.gain.load(Ordering::Relaxed));
                    let speed = f32::from_bits(track.speed.load(Ordering::Relaxed));
                    
                    let output_block = buffer.len() / track.channels;
                    let loop_length = (track.end - track.start);
                    let crossfade_samples = 250;
                    
                    for frame in 0..output_block {
                        let mut relative_position = (track.position + speed)
                            .rem_euclid(loop_length as f32);
                        if relative_position.is_nan() { relative_position = 0.0; }
                        
                        let index    =  relative_position.floor() as usize;
                        let fraction = (relative_position - (index as f32)).clamp(0.0, 1.0);
                        let next_index = if index + 1 >= loop_length {
                            0
                        } else {
                            index + 1
                        };
                        
                        let base_offset = (track.start + index)
                            .checked_mul(track.channels)
                            .unwrap_or_else(|| {
                                panic!(
                                    "overflow: {} * {} too big for usize",
                                    (track.start + index), track.channels
                                )
                            });
                        
                        let next_offset = (track.start + next_index)
                            .checked_mul(track.channels)
                            .unwrap_or_else(|| {
                                panic!(
                                    "overflow: {} * {} too big for usize",
                                    (track.start + next_index), track.channels
                                )
                            });

                        for channel in 0..track.channels {
                            let s0 = *track
                                .samples
                                .get(base_offset + channel)
                                .unwrap_or(&0.0);
                            let s1 = *track
                                .samples
                                .get(next_offset + channel)
                                .unwrap_or(&0.0);
                            let mut sample = s0 + (s1 - s0) * fraction;


                            if index >= loop_length - crossfade_samples {
                                let fade_pos = (index + 1 + crossfade_samples - loop_length) as f32 / crossfade_samples as f32;
                                let fade_pos = fade_pos.clamp(0.0, 1.0);

                                let start_sample = *track.samples.get(track.start * track.channels + channel).unwrap_or(&0.0);
                                sample = sample * (1.0 - fade_pos) + start_sample * fade_pos;
                            }
                            
                            
                            buffer[frame*track.channels + channel] += sample * gain;
                        }

                        track.position = relative_position;

                        //
                        // // Clip
                        // for s in buffer.iter_mut() {
                        //     *s = (*s).clamp(-1.0, 1.0);
                        // }
                    }
                }
                _ => {
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
            let track = Track{ samples, position: 0.0, start: 0, end, channels, gain: AtomicU32::new(1u32), speed: AtomicU32::new(1u32) };
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
                    let mut t = track;
                    t.position = t.start as f32;
                    EngineState::Playing (t)
                }

                (EngineState::Ready (track), EngineEvent::SetStart(start)) => {
                    let mut t = track;
                    let start_samples = start * (t.samples.len() / t.channels) as f32;
                    t.start = start_samples as usize;
                    t.position = start_samples;
                    EngineState::Ready (t)
                }
                (EngineState::Ready (track), EngineEvent::SetEnd(end)) => {
                    let mut t = track;
                    t.end = (end * (t.samples.len() / t.channels) as f32) as usize;
                    t.position = t.start as f32;
                    EngineState::Ready (t)
                }

                // ─── Playing ───
                (EngineState::Playing (track), EngineEvent::Pause) => {
                    EngineState::Paused (track)
                }
                (EngineState::Playing (track), EngineEvent::Stop) => {
                    EngineState::Ready (track)
                }
                (EngineState::Playing (track), EngineEvent::SetGain(g)) => {
                    let t = track;
                    t.gain.store(g.to_bits(), Ordering::Relaxed);
                    EngineState::Playing (t)
                }
                (EngineState::Playing (track), EngineEvent::SetSpeed(s)) => {
                    let t = track;
                    t.speed.store(s.to_bits(), Ordering::Relaxed);
                    EngineState::Playing (t)
                }

                (EngineState::Playing (track), EngineEvent::SetStart(start)) => {
                    let mut t = track;
                    let start_samples = start * t.samples.len() as f32;
                    t.start = start_samples as usize;
                    t.position = start_samples;
                    EngineState::Paused (t)
                }
                (EngineState::Playing (track), EngineEvent::SetEnd(end)) => {
                    let mut t = track;
                    t.end = (end * (t.samples.len() / t.channels) as f32) as usize;
                    t.position = t.start as f32;
                    EngineState::Paused (t)
                }
                
                (EngineState::Paused (track), EngineEvent::Play) => {
                    let mut t = track;
                    t.position = t.start as f32;
                    EngineState::Playing (t)
                }
                
                (EngineState::Paused (track), EngineEvent::Stop) => {
                    EngineState::Ready (track)
                }
                (EngineState::Paused (track), EngineEvent::SetGain(g)) => {
                    let t = track;
                    t.gain.store(g.to_bits(), Ordering::Relaxed);
                    EngineState::Paused (t)
                }
                
                (EngineState::Paused (track), EngineEvent::SetSpeed(s)) => {
                    let t = track;
                    t.speed.store(s.to_bits(), Ordering::Relaxed);
                    EngineState::Paused (t)
                }

                (EngineState::Paused (track), EngineEvent::SetStart(start)) => {    
                    let mut t = track;
                    let start_samples = start * (t.samples.len() / t.channels) as f32;
                    t.start = start_samples as usize;
                    t.position = start_samples;
                    EngineState::Paused (t)
                }
                (EngineState::Paused (track), EngineEvent::SetEnd(end)) => {
                    let mut t = track;
                    t.end = (end * (t.samples.len() / t.channels) as f32) as usize;
                    t.position = t.start as f32;
                    EngineState::Paused (t)
                }

                // no state change
                (old_state, _) => old_state,
            };
        }
    }
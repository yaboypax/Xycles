
    use rodio::{Decoder, source::Source};
    use crossbeam_queue::ArrayQueue;
    use std::{fs::File, io::BufReader, mem};
    use std::sync::Arc;

    pub use crate::track::Track;
    use crate::track::grain::Grain;
    use crate::track::play_head::PlayHead;
    use crate::track::grain::GrainHead;

    pub enum EngineState {
        Idle,
        Ready (Track),
        Playing (Track),
        Granulating (Track),
        Paused (Track),
    }
    
    pub enum ParameterState {
        SetGain(f32),
        SetSpeed(f32),
        SetStart(f32),
        SetEnd(f32),

        SetGrainSpeed(f32),
        SetGrainLength(f32),
        SetGrainOverlap(f32),
        SetGrainCount(f32),
        SetGrainSpread(f32)
    }
    pub enum EngineEvent {
        Load(String),
        Play,           
        Pause,          
        Stop,

        SetParameters(ParameterState)
    }

    pub struct Engine {
        state: EngineState,
        commands: Arc<ArrayQueue<EngineEvent>>,
    }

    impl Engine {

        pub fn new() -> Self {
            Engine { state: EngineState::Idle, commands: Arc::new(ArrayQueue::new(32))}
        }
        
        // FFI Bridge
        pub fn load_audio(&mut self, path: &str)   { self.push_event(EngineEvent::Load(path.into())) }
        pub fn play(&mut self)                     { self.push_event(EngineEvent::Play) }
        pub fn pause(&mut self)                    { self.push_event(EngineEvent::Pause) }
        pub fn stop(&mut self)                     { self.push_event(EngineEvent::Stop) }
        
        pub fn set_gain(&mut self, g: f32)         { self.push_event(EngineEvent::SetParameters(ParameterState::SetGain(g)))}
        pub fn set_speed(&mut self, s: f32)        { self.push_event(EngineEvent::SetParameters(ParameterState::SetSpeed(s)))}
        pub fn set_start(&mut self, start: f32)    { self.push_event(EngineEvent::SetParameters(ParameterState::SetStart(start)))}
        pub fn set_end(&mut self, end: f32)        { self.push_event(EngineEvent::SetParameters(ParameterState::SetEnd(end)))}

        pub fn set_grain_speed(&mut self, s: f32)  { self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainSpeed(s)))}
        pub fn set_grain_length(&mut self, l: f32) { self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainLength(l)))}
        pub fn set_grain_overlap(&mut self, o: f32){ self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainOverlap(o)))}
        pub fn set_grain_count(&mut self, c: f32)  { self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainCount(c)))}
        pub fn set_grain_spread(&mut self, sp: f32){ self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainSpread(sp)))}
        
        pub fn get_playhead(&self) -> f32
        {
            match &self.state {
                EngineState::Playing(track) | EngineState::Paused(track)
                | EngineState::Ready(track) => {
                    if track.start < track.end {
                        let loop_length = (track.end - track.start) as f32;
                        let play_head = track.play_head();
                        play_head.position.clamp(0.0, loop_length) / loop_length
                    } else {
                         0.0
                    }
                }
                EngineState::Idle => 0.0,
                EngineState::Granulating(_track) => 0.0
            }
        }
        
        pub fn fill_silence(buffer: &mut Vec<f32>) {for sample in buffer.iter_mut() {*sample = 0.0}; }

        fn push_event(&self, event: EngineEvent) {
            let _ = self.commands.push(event);
        }
        fn apply_pending(&mut self) {
            while let Some(event) = self.commands.pop() {
                self.transition(event);
            }
        }

        pub fn process_block(&mut self, buffer: &mut Vec<f32>)
        {
            self.apply_pending();
            match &mut self.state {

                EngineState::Granulating(track ) => {
                    track.granular_process_block(buffer);
                }

                EngineState::Playing (track) => {
                    track.playhead_proccess_block(buffer);
                }
                _ => {
                }
            }
            
        }

        pub fn load_path(&mut self, path: String) -> EngineState
        {
            let file   = BufReader::new(File::open(&path).unwrap());
            let source = Decoder::new(file).unwrap();
            let sample_rate = source.sample_rate() as usize;
            let channels = source.channels() as usize;
            let samples: Vec<f32> = source.convert_samples().collect();
            let end = samples.len() / channels;

            let track = Track{ samples, start: 0, end, channels, play_head: PlayHead::new(), grain_head: GrainHead::new(sample_rate)};
            EngineState::Ready(track)
        }
        


        
        pub fn transition(&mut self, event: EngineEvent) {
            let old = mem::replace(&mut self.state, EngineState::Idle);
            self.state = match (old, event) {

                // ─── Idle ───
                (_, EngineEvent::Load(path)) => {
                    self.load_path(path)
                }
                
                (EngineState::Ready (track), EngineEvent::Play) => {
                    let mut t = track;
                    t.play_head_mut().position = t.start as f32;
                    EngineState::Granulating (t)
                }

                (EngineState::Ready (track), EngineEvent::SetParameters(state )) => {
                    let mut t = track;
                    t.set_parameters(state);
                    EngineState::Ready (t)
                }

                // ─── Playing ───
                (EngineState::Playing (track), EngineEvent::Stop) => {
                    EngineState::Ready (track)
                }
                (EngineState::Playing (track), EngineEvent::SetParameters(state )) => {
                    let mut t = track;
                    t.set_parameters(state);
                    EngineState::Playing (t)
                }

                // ─── Granulating ───

                (EngineState::Granulating(track), EngineEvent::Stop) => {
                    EngineState::Ready(track)
                }
                (EngineState::Granulating(track), EngineEvent::SetParameters(state)) => {
                    let mut t = track;
                    t.set_parameters(state);
                    EngineState::Granulating(t)
                }

                

                // no state change
                (old_state, _) => old_state,
            };
        }
    }
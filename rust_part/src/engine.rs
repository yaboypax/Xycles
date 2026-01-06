use crossbeam_queue::ArrayQueue;
use rodio::{source::Source, Decoder};
use std::sync::Arc;
use std::{fs::File, io::BufReader, mem};

use crate::track::grain::GrainHead;
use crate::track::play_head::PlayHead;
pub use crate::track::Track;

use freeverb::Freeverb;

pub enum EngineState {
    Idle,
    Ready(Track),
    Playing(Track),
    Granulating(Track),
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
    SetGrainSpread(f32),

    SetReverbWet(f64),
    SetReverbSize(f64),
    SetReverbDamp(f64),
}

pub enum EngineEvent {
    Load(String),
    Play,
    GrainPlay,
    Stop,

    SetParameters(ParameterState),
}

pub struct Engine {
    state: EngineState,
    commands: Arc<ArrayQueue<EngineEvent>>,
}

impl Engine {
    pub fn new() -> Self {
        Engine {
            state: EngineState::Idle,
            commands: Arc::new(ArrayQueue::new(32)),
        }
    }

    // FFI Bridge
    pub fn load_audio(&mut self, path: &str) {
        self.push_event(EngineEvent::Load(path.into()))
    }
    pub fn play(&mut self) {
        self.push_event(EngineEvent::Play)
    }
    pub fn grain_play(&mut self) {
        self.push_event(EngineEvent::GrainPlay)
    }
    pub fn stop(&mut self) {
        self.push_event(EngineEvent::Stop)
    }

    pub fn set_gain(&mut self, g: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetGain(g)))
    }
    pub fn set_speed(&mut self, s: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetSpeed(s)))
    }
    pub fn set_start(&mut self, start: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetStart(start)))
    }
    pub fn set_end(&mut self, end: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetEnd(end)))
    }

    pub fn set_grain_speed(&mut self, s: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainSpeed(s)))
    }
    pub fn set_grain_length(&mut self, l: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainLength(
            l,
        )))
    }
    pub fn set_grain_overlap(&mut self, o: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainOverlap(
            o,
        )))
    }
    pub fn set_grain_count(&mut self, c: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainCount(c)))
    }
    pub fn set_grain_spread(&mut self, sp: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetGrainSpread(
            sp,
        )))
    }

    pub fn set_reverb_wet(&mut self, wet: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetReverbWet(
            wet as f64,
        )))
    }
    pub fn set_reverb_size(&mut self, size: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetReverbSize(
            size as f64,
        )))
    }
    pub fn set_reverb_damp(&mut self, damp: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetReverbDamp(
            damp as f64,
        )))
    }

    pub fn get_playhead(&self) -> f32 {
        match &self.state {
            EngineState::Playing(track) | EngineState::Ready(track) => {
                if track.start < track.end {
                    let loop_length = (track.end - track.start) as f32;
                    let play_head = track.play_head();
                    play_head.position.clamp(0.0, loop_length) / loop_length
                } else {
                    0.0
                }
            }
            EngineState::Idle => 0.0,
            EngineState::Granulating(track) => {
                if track.start < track.end {
                    let loop_length = (track.end - track.start) as f32;
                    let grain_head = track.grain_head();
                    grain_head.base_pos.clamp(0.0, loop_length) / loop_length
                } else {
                    0.0
                }
            }
        }
    }

    pub fn get_length(&self) -> usize {
        match &self.state {
            EngineState::Playing(track)
            | EngineState::Ready(track)
            | EngineState::Granulating(track) => track.samples.len() / track.channels,
            EngineState::Idle => 0,
        }
    }

    pub fn fill_silence(buffer: &mut [f32]) {
        for sample in buffer.iter_mut() {
            *sample = 0.0
        }
    }

    fn push_event(&self, event: EngineEvent) {
        let _ = self.commands.push(event);
    }
    fn apply_pending(&mut self) {
        while let Some(event) = self.commands.pop() {
            self.transition(event);
        }
    }

    pub fn process_block(&mut self, buffer: &mut Vec<f32>) {
        self.apply_pending();
        match &mut self.state {
            EngineState::Granulating(track) => {
                track.granular_process_block(buffer);
            }
            EngineState::Playing(track) => {
                track.playhead_proccess_block(buffer);
            }
            _ => {}
        }
    }

    pub fn load_path(&mut self, path: String) -> Track {
        let file = BufReader::new(File::open(&path).unwrap());
        let source = Decoder::new(file).unwrap();
        let sample_rate = source.sample_rate() as usize;
        let channels = source.channels() as usize;
        let samples: Vec<f32> = source.convert_samples().collect();
        let end = samples.len() / channels;
        Track {
            samples,
            start: 0,
            end,
            channels,
            play_head: PlayHead::new(),
            grain_head: GrainHead::new(sample_rate),
            reverb: Freeverb::new(sample_rate),
        }
    }

    pub fn transition(&mut self, event: EngineEvent) {
        let old = mem::replace(&mut self.state, EngineState::Idle);
        self.state = match (old, event) {
            // ─── Idle ───
            (_, EngineEvent::Load(path)) => {
                let track = self.load_path(path);
                EngineState::Ready(track)
            }
            (EngineState::Ready(track), EngineEvent::Play) => {
                let mut t = track;
                t.play_head_mut().position = t.start as f32;
                EngineState::Playing(t)
            }
            (EngineState::Ready(track), EngineEvent::GrainPlay) => {
                let mut t = track;
                t.grain_head_mut().base_pos = t.start as f32;
                EngineState::Granulating(t)
            }
            (EngineState::Ready(track), EngineEvent::SetParameters(state)) => {
                let mut t = track;
                t.set_parameters(state);
                EngineState::Ready(t)
            }

            // ─── Playing ───
            (EngineState::Playing(track), EngineEvent::Stop) => EngineState::Ready(track),
            (EngineState::Playing(track), EngineEvent::SetParameters(state)) => {
                let mut t = track;
                t.set_parameters(state);
                EngineState::Playing(t)
            }
            (EngineState::Playing(track), EngineEvent::GrainPlay) => {
                let mut t = track;
                t.grain_head_mut().base_pos = t.play_head().position;
                EngineState::Granulating(t)
            }

            // ─── Granulating ───
            (EngineState::Granulating(track), EngineEvent::Stop) => EngineState::Ready(track),
            (EngineState::Granulating(track), EngineEvent::SetParameters(state)) => {
                let mut t = track;
                t.set_parameters(state);
                EngineState::Granulating(t)
            }
            (EngineState::Granulating(track), EngineEvent::Play) => {
                let mut t = track;
                t.play_head_mut().position = t.grain_head().base_pos;
                EngineState::Playing(t)
            }

            // no state change
            (old_state, _) => old_state,
        };
    }
}

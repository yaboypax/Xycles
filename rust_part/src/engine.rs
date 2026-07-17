use aych_delay::{Delay, Settings};
use crossbeam_queue::ArrayQueue;
use rodio::{source::Source, Decoder};
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::{fs::File, io::BufReader, mem};

use crate::track::grain::GrainHead;
use crate::track::play_head::PlayHead;
pub use crate::track::Track;

use freeverb::Freeverb;

pub enum EngineState {
    Idle,
    Ready(Box<Track>),
    Playing(Box<Track>),
    Granulating(Box<Track>),
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

    SetDelayWet(f32),
    SetDelayTime(f32),
    SetDelayFeedback(f32),
}

pub enum EngineEvent {
    Load(Box<Track>),
    Play,
    GrainPlay,
    Stop,

    SetParameters(ParameterState),
}

pub struct Engine {
    state: EngineState,
    commands: Arc<ArrayQueue<EngineEvent>>,
    loading: Arc<AtomicBool>,
    retired: Arc<ArrayQueue<Box<Track>>>,
}

impl Engine {
    pub fn new() -> Self {
        Engine {
            state: EngineState::Idle,
            commands: Arc::new(ArrayQueue::new(32)),
            loading: Arc::new(AtomicBool::new(false)),
            retired: Arc::new(ArrayQueue::new(32)),
        }
    }

    // FFI Bridge
    pub fn load_audio(&mut self, path: &str) {
        let commands = Arc::clone(&self.commands);
        let loading = Arc::clone(&self.loading);
        let retired = Arc::clone(&self.retired);
        let path = path.to_string();
        loading.store(true, Ordering::Relaxed);
        std::thread::spawn(move || {
            while retired.pop().is_some() {}
            if let Some(track) = Engine::load_path(path) {
                let _ = commands.push(EngineEvent::Load(Box::new(track)));
            }
            loading.store(false, Ordering::Relaxed);
        });
    }
    pub fn is_loading(&self) -> bool {
        self.loading.load(Ordering::Relaxed)
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

    pub fn set_delay_wet(&mut self, wet: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetDelayWet(wet)))
    }

    pub fn set_delay_time(&mut self, time: f32) {
        self.push_event(EngineEvent::SetParameters(ParameterState::SetDelayTime(
            time,
        )))
    }

    pub fn set_delay_feedback(&mut self, feedback: f32) {
        self.push_event(EngineEvent::SetParameters(
            ParameterState::SetDelayFeedback(feedback),
        ))
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

    pub fn get_path(&self) -> String {
        match &self.state {
            EngineState::Playing(track)
            | EngineState::Ready(track)
            | EngineState::Granulating(track) => track.path.clone(),
            EngineState::Idle => String::new(),
        }
    }

    // interleaved [f32] {position, alpha}
    pub fn fill_grains(&self, out: &mut [f32]) -> usize {
        let view = match &self.state {
            EngineState::Granulating(track) => &track.grain_head.view,
            _ => return 0,
        };
        let mut n = view.len().min(out.len());
        n -= n % 2;
        out[..n].copy_from_slice(&view[..n]);
        n / 2
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

    pub fn process_block(&mut self, buffer: &mut [f32]) {
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

    pub fn load_path(path: String) -> Option<Track> {
        let file = BufReader::new(File::open(&path).ok()?);
        let source = Decoder::new(file).ok()?;
        let sample_rate = source.sample_rate() as usize;
        let channels = source.channels() as usize;
        let samples: Vec<f32> = source.convert_samples().collect();
        let end = samples.len() / channels;
        let mut reverb = Freeverb::new(sample_rate);
        reverb.set_wet(0.0);
        reverb.set_dry(1.0);
        Some(Track {
            path,
            samples,
            start: 0,
            end,
            channels,
            play_head: PlayHead::new(),
            grain_head: GrainHead::new(sample_rate),
            reverb,
            delay: Delay::new(Settings {
                delay_time: 1666.66,
                feedback: 0.1,
                width: 0.5,
                lowpass_filter: 22000.0,
                highpass_filter: 30.0,
                dry_wet_mix: 0.0,
                output_level: 1.0,
                sample_rate: sample_rate as f32,
                ..Settings::default()
            }),
        })
    }

    pub fn transition(&mut self, event: EngineEvent) {
        let old = mem::replace(&mut self.state, EngineState::Idle);
        self.state = match (old, event) {
            // ─── Idle ───
            (old_state, EngineEvent::Load(track)) => {
                match old_state {
                    EngineState::Ready(old_track)
                    | EngineState::Playing(old_track)
                    | EngineState::Granulating(old_track) => {
                        let _ = self.retired.push(old_track);
                    }
                    EngineState::Idle => {}
                }
                EngineState::Ready(track)
            }
            (EngineState::Ready(track), EngineEvent::Play) => {
                let mut t = track;
                t.play_head_mut().position = 0.0;
                EngineState::Playing(t)
            }
            (EngineState::Ready(track), EngineEvent::GrainPlay) => {
                let mut t = track;
                t.grain_head_mut().base_pos = 0.0;
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

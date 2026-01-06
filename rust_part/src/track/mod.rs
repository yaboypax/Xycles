pub mod grain;
pub mod play_head;

use crate::engine::ParameterState;
use crate::track::grain::Grain;
use grain::GrainHead;
use play_head::PlayHead;

use freeverb::Freeverb;

#[inline]
fn rando(state: &mut u32) -> f32 {
    *state = state.wrapping_mul(1664525).wrapping_add(1013904223);
    ((*state >> 8) as f32) * (1.0 / 16_777_216.0)
}

#[inline]
fn random_pan(spread: f32, state: &mut u32) -> f32 {
    let u = 2.0 * rando(state) - 1.0; // [-1, 1)
    (u * spread).clamp(-1.0, 1.0)
}

#[inline]
fn equal_power_gains(pan: f32) -> (f32, f32) {
    // pan in [-1,1] → theta in [0, π/2]
    let theta = (pan.clamp(-1.0, 1.0) + 1.0) * std::f32::consts::FRAC_PI_4;
    (theta.cos(), theta.sin()) // (left, right)
}

pub struct Track {
    pub samples: Vec<f32>,
    pub start: usize,
    pub end: usize,
    pub channels: usize,
    pub play_head: PlayHead,
    pub grain_head: GrainHead,
    pub reverb: Freeverb,
}

impl Track {
    pub fn set_parameters(&mut self, state: ParameterState) {
        let samples = self.samples.len();
        let channels = self.channels;
        //self.reverb.set_room_size(2.0);

        match state {
            ParameterState::SetStart(start) => {
                let start_samples = start * (samples / channels) as f32;
                self.start = start_samples as usize;
                self.play_head_mut().position = start_samples;
            }
            ParameterState::SetEnd(end) => {
                self.end = (end * (self.samples.len() / self.channels) as f32) as usize;
                self.play_head_mut().position = self.start as f32;
            }

            ParameterState::SetGain(g) => {
                self.play_head_mut().gain = g;
                self.grain_head_mut().gain = g;
            }
            ParameterState::SetSpeed(s) => {
                self.play_head_mut().speed = s;
                self.grain_head_mut().speed = s;
            }

            ParameterState::SetGrainSpeed(grain_speed) => {
                self.grain_head_mut().grain_speed = grain_speed;
            }

            ParameterState::SetGrainLength(grain_length) => {
                let grain_size = grain_length as usize;
                let grainhead = self.grain_head_mut();
                grainhead.grain_size = grain_size;
                grainhead.window = GrainHead::calculate_window(grain_size);
            }

            ParameterState::SetGrainOverlap(grain_overlap) => {
                let grainhead = self.grain_head_mut();
                grainhead.overlap = grain_overlap;
                grainhead.hop_size = ((grainhead.grain_size as f32) / grain_overlap)
                    .max(1.0)
                    .round() as usize;
            }
            ParameterState::SetGrainCount(grain_count) => {
                self.grain_head_mut().count = grain_count as usize;
            }

            ParameterState::SetReverbWet(wet) => {
                self.reverb.set_wet(wet);
                self.reverb.set_dry(1.0 - wet);
            }
            ParameterState::SetReverbSize(size) => {
                self.reverb.set_room_size(size);
            }

            ParameterState::SetReverbDamp(damp) => {
                self.reverb.set_dampening(damp);
            }
            _ => {}
        }
    }

    pub fn play_head(&self) -> &PlayHead {
        &self.play_head
    }
    pub fn play_head_mut(&mut self) -> &mut PlayHead {
        &mut self.play_head
    }

    pub fn playhead_proccess_block(&mut self, buffer: &mut [f32]) {
        let output_block = buffer.len() / self.channels;
        let loop_length = self.end - self.start;
        let crossfade_samples = 250;

        for frame in 0..output_block {
            let mut relative_position = (self.play_head_mut().position
                + self.play_head_mut().speed)
                .rem_euclid(loop_length as f32);
            if relative_position.is_nan() {
                relative_position = 0.0;
            }

            let index = relative_position.floor() as usize;
            let fraction = (relative_position - (index as f32)).clamp(0.0, 1.0);
            let next_index = if index + 1 >= loop_length {
                0
            } else {
                index + 1
            };

            let base_offset = (self.start + index)
                .checked_mul(self.channels)
                .unwrap_or_else(|| {
                    panic!(
                        "overflow: {} * {} too big for usize",
                        (self.start + index),
                        self.channels
                    )
                });

            let next_offset = (self.start + next_index)
                .checked_mul(self.channels)
                .unwrap_or_else(|| {
                    panic!(
                        "overflow: {} * {} too big for usize",
                        (self.start + next_index),
                        self.channels
                    )
                });

            for channel in 0..self.channels {
                let s0 = *self.samples.get(base_offset + channel).unwrap_or(&0.0);
                let s1 = *self.samples.get(next_offset + channel).unwrap_or(&0.0);

                let (left_wet, right_wet) = self.reverb.tick((s0 as f64, s1 as f64));
                let mut sample = left_wet as f32 + (right_wet - left_wet) as f32 * fraction;

                if index >= loop_length - crossfade_samples {
                    let fade_pos = (index + 1 + crossfade_samples - loop_length) as f32
                        / crossfade_samples as f32;
                    let fade_pos = fade_pos.clamp(0.0, 1.0);

                    let start_sample = *self
                        .samples
                        .get(self.start * self.channels + channel)
                        .unwrap_or(&0.0);
                    sample = sample * (1.0 - fade_pos) + start_sample * fade_pos;
                }

                buffer[frame * self.channels + channel] += sample * self.play_head_mut().gain;
            }

            self.play_head_mut().position = relative_position;

            //
            // // Clip
            // for s in buffer.iter_mut() {
            //     *s = (*s).clamp(-1.0, 1.0);
            // }
        }
    }

    pub fn grain_head(&self) -> &GrainHead {
        &self.grain_head
    }
    pub fn grain_head_mut(&mut self) -> &mut GrainHead {
        &mut self.grain_head
    }

    pub fn granular_process_block(&mut self, buffer: &mut [f32]) {
        let channels = self.channels;
        let frames = buffer.len() / channels;
        let start = self.start;
        let end = self.end;
        let loop_len = end - start;

        // mutable object refs
        let (ghm, reverb, samples) = (&mut self.grain_head, &mut self.reverb, &self.samples);

        // immutable value refs
        let (
            mut grains,
            hop_size,
            speed,
            grain_speed,
            spread,
            mut rng_state,
            mut read_position,
            mut spawn_ctr,
            window,
            grain_size,
            gain,
            count,
        ) = {
            let grains = std::mem::take(ghm.grains());
            let hop_size = ghm.hop_size;
            let speed = ghm.speed;
            let grain_speed = ghm.grain_speed;
            let spread = ghm.spread;
            let rng_state = ghm.rng_state;
            let read_position = ghm.base_pos;
            let spawn_ctr = std::mem::take(&mut ghm.spawn);

            let window: &[f32] = &ghm.window;
            let grain_size = ghm.grain_size;
            let gain = ghm.gain;
            let count = ghm.count;

            (
                grains,
                hop_size,
                speed,
                grain_speed,
                spread,
                rng_state,
                read_position,
                spawn_ctr,
                window,
                grain_size,
                gain,
                count,
            )
        };

        for frame in 0..frames {
            // every hop_size frames spawn a new grain at read_pos=0
            if spawn_ctr == 0 {
                for _ in 0..count {
                    let pan = random_pan(spread, &mut rng_state);
                    grains.push(Grain {
                        // anchor the grain at current head position inside the loop
                        read_position,
                        window_position: 0,
                        pan,
                        grain_speed,
                    });

                    read_position += 10.0;
                }
                spawn_ctr = hop_size;
                if grains.len() > 64 {
                    grains.remove(0);
                }
            }
            spawn_ctr = spawn_ctr.saturating_sub(1);

            // advance each grain and sum into output
            let (mut out_left, mut out_right): (f32, f32) = (0.0, 0.0);
            grains.retain_mut(|g| {
                if g.window_position < grain_size {
                    let idx = (g.read_position.floor() as usize) % loop_len;
                    let next = (idx + 1) % loop_len;
                    let frac = (g.read_position - (idx as f32)).clamp(0.0, 1.0);
                    let w = window[g.window_position];

                    let s0_l = *samples.get((start + idx) * 2).unwrap_or(&0.0);
                    let s0_r = *samples.get((start + idx) * 2 + 1).unwrap_or(&0.0);
                    let s1_l = *samples.get((start + next) * 2).unwrap_or(&0.0);
                    let s1_r = *samples.get((start + next) * 2 + 1).unwrap_or(&0.0);

                    let l = s0_l + (s1_l - s0_l) * frac;
                    let r = s0_r + (s1_r - s0_r) * frac;

                    let mono = 0.5 * (l + r);
                    let (gl, gr) = equal_power_gains(g.pan);

                    out_left += mono * w * gain * gl;
                    out_right += mono * w * gain * gr;

                    g.read_position = (g.read_position + g.grain_speed).rem_euclid(loop_len as f32);
                    g.window_position += 1;
                    true
                } else {
                    false
                }
            });

            let (left_wet, right_wet) = reverb.tick((out_left as f64, out_right as f64));
            buffer[frame * channels] += left_wet as f32;
            buffer[frame * channels + 1] += right_wet as f32;

            read_position += speed;
            if read_position >= loop_len as f32 {
                read_position -= loop_len as f32;
            }
            if read_position < 0.0 {
                read_position += loop_len as f32;
            }
        }

        let ghm = self.grain_head_mut();
        ghm.base_pos = read_position;
        ghm.spawn = spawn_ctr;
        ghm.rng_state = rng_state;
        *ghm.grains() = grains;
    }
}

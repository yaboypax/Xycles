
pub struct Grain {
    pub read_position: f32,
    pub window_position: usize,
    pub grain_speed: f32,
    pub velocity: f32
}

pub struct GrainHead {
    pub grain_size:  usize,
    pub hop_size:    usize,
    pub window:      Vec<f32>,   // precomputed Hann window of length grain_size
        grains:      Vec<Grain>,
    pub gain:        f32,
    pub speed:       f32,
}

impl GrainHead {
    pub fn new(sample_rate: usize) -> GrainHead {

        let grain_size = 200 * (sample_rate/ 1000); // ms to samples
        let hop_size = (grain_size as f32 / 0.25) as usize;
        GrainHead { grain_size, hop_size, window: GrainHead::calculate_window(grain_size), grains: Vec::new(), gain: 1.0, speed: 1.0}
    }

    pub fn calculate_window(grain_size: usize) -> Vec<f32> {
       (0..grain_size)
            .map(|i| {
                let x = i as f32 / (grain_size - 1) as f32;
                0.5 * (1.0 - (2.0 * std::f32::consts::PI * x).cos())
            })
            .collect()
    }

    pub fn grains(&mut self) -> &mut Vec<Grain> {
        &mut self.grains
    }
}

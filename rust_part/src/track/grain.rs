pub struct Grain {
    pub read_position: f32,
    pub window_position: usize,
    pub grain_speed: f32,
    pub pan: f32
}

pub struct GrainHead {
    pub grain_size:  usize,
    pub hop_size:    usize,
    pub window:      Vec<f32>,   // precomputed Hann window of length grain_size
        grains:      Vec<Grain>,
    pub gain:        f32,
    pub speed:       f32,
    pub grain_speed: f32,
    pub overlap:     f32,    
    pub base_pos:    f32,       
    pub spawn:       usize,
    pub count:       usize,
    pub spread:      f32,
    pub rng_state:   u32,
    pub view:        Vec<f32>,
}

impl GrainHead {
    pub fn new(sample_rate: usize) -> GrainHead {

        let grain_ms   = 200.0;
        let grain_size = (sample_rate as f32 * (grain_ms / 1000.0)).round() as usize;

        let overlap  = 4.0;
        let hop_size   = ((grain_size as f32) / overlap).max(1.0).round() as usize;

        GrainHead {
            grain_size,
            hop_size,
            window: Self::calculate_window(grain_size),
            grains: Vec::new(),
            gain: 1.0,
            speed: 1.0,
            grain_speed: 1.0,
            overlap,
            base_pos: 0.0,
            spawn: 0,
            count: 1,
            spread: 1.0,
            rng_state: 0,
            view: Vec::with_capacity(256),
        }}

    pub fn snapshot(&mut self, loop_len: usize) {
        self.view.clear();
        for g in &self.grains {
            let position = if loop_len > 0 {
                g.read_position / loop_len as f32
            } else {
                0.0
            };
            let alpha = self.window.get(g.window_position).copied().unwrap_or(0.0);
            self.view.push(position);
            self.view.push(alpha);
        }
    }

    pub fn calculate_window(grain_size: usize) -> Vec<f32> {
        let denom = (grain_size - 1) as f32;
        (0..grain_size)
            .map(|i| {
                let theta = std::f32::consts::PI * (i as f32 / denom);
                let s = theta.sin();
                s * s // sin^2(π x), identical to 0.5*(1 - cos(2πx)) but more stable
            })
            .collect()
    }

    pub fn grains(&mut self) -> &mut Vec<Grain> {
        &mut self.grains
    }
}

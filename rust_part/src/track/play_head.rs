pub struct PlayHead {
    pub position: f32,
    pub gain: f32,
    pub speed: f32,
}

impl PlayHead {
    pub fn new() -> Self {
        PlayHead {position: 0.0, gain: 1.0, speed: 1.0 }
    }
}

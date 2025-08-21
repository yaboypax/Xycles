pub mod grain;
pub mod play_head;

use grain::GrainHead;
use play_head::PlayHead;

pub struct Track {
    pub samples:    Vec<f32>,
    pub start:      usize,
    pub end:        usize,
    pub channels:   usize,
    pub play_head:  PlayHead,
    pub grain_head: GrainHead
}

impl Track {
    pub fn play_head(&self) -> &PlayHead {
         &self.play_head
    }
    pub fn play_head_mut(&mut self) -> &mut PlayHead {
        &mut self.play_head
    }

    pub fn grain_head(&self) -> &GrainHead {
        &self.grain_head
    }
    pub fn grain_head_mut(&mut self) -> &mut GrainHead { &mut self.grain_head}
}
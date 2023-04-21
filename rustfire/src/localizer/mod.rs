mod icp;
mod noop;

use std::sync::mpsc;

use ordered_float::OrderedFloat;

use crate::math::{Point, Transform};
use crate::Map;

pub use self::icp::icp_localizer;
pub use self::noop::noop_localizer;

/// A localizer.
pub struct Localizer {
    channel: mpsc::Receiver<Option<Transform>>,
}

impl Localizer {
    /// Creates a localizer with the specified handler, which should update the
    /// value in the channel when the guess changes
    fn from_handler<H>(handler: H) -> Self
    where
        H: FnOnce(mpsc::SyncSender<Option<Transform>>) + Send + 'static,
    {
        let (send, recv) = mpsc::sync_channel(1);

        std::thread::spawn(|| handler(send));

        Localizer { channel: recv }
    }

    /// Waits for the next position to be available, and returns it.
    pub fn next_position(&mut self) -> Option<Transform> {
        self.channel.recv().unwrap()
    }
}

/// The cost of the guess. Equal to the square root of the average squared
/// distance to the walls.
pub fn cost(position: Transform, points: &[Point], map: &Map) -> f32 {
    let mut distances: Vec<_> = points
        .iter()
        .map(|&point| {
            let point = position * point;
            (map.point_closest_to(point) - point).length_sq()
        })
        .collect();
    distances.sort_by_key(|&v| OrderedFloat(v));

    let to_consider = &distances[..distances.len() * 19 / 20];
    let sum: f32 = to_consider.iter().sum();
    (sum / to_consider.len() as f32).sqrt()
}

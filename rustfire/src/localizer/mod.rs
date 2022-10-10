mod icp;
mod noop;

use std::sync::mpsc;

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
    let mut sum = 0.0;

    for &point in points {
        let point = position * point;
        sum += (map.point_closest_to(point) - point).length_sq();
    }

    (sum / points.len() as f32).sqrt()
}

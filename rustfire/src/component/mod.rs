mod cameras;
mod find;
mod real_lidar;
mod serial;
mod simulated;
mod wheels;

use std::sync::mpsc::{sync_channel, Receiver, SyncSender};

use crate::math::Point;

pub use self::cameras::{Cameras, State};
pub use self::find::find_components;
pub use self::real_lidar::real_lidar;
pub use self::serial::Serial;
pub use self::simulated::simulated_lidar;
pub use self::wheels::Wheels;

/// A lidar.
pub struct Lidar {
    channel: Receiver<Vec<Point>>,
}

impl Lidar {
    /// Creates a lidar with the specified handler, which should watch the lidar and
    /// update the value in the channel on each scan.
    fn from_handler<H>(handler: H) -> Self
    where
        H: FnOnce(SyncSender<Vec<Point>>) + Send + 'static,
    {
        let (send, recv) = sync_channel(1);

        std::thread::spawn(|| handler(send));

        Lidar { channel: recv }
    }

    /// Waits for the next scan to be available, and then returns the scanned points.
    pub fn next_scan(&mut self) -> Vec<Point> {
        self.channel.recv().unwrap()
    }
}

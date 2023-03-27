mod real_lidar;
mod serial;
mod simulated;
mod real_cameras;

use std::sync::mpsc::{Receiver, SyncSender, sync_channel};

use crate::math::Point;

pub use self::real_lidar::real_lidar;
pub use self::real_cameras::real_cameras;
pub use self::serial::Serial;
pub use self::simulated::simulated_lidar;

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

/// A set of infrared cameras.
pub struct Cameras {
    channel: Receiver<Option<Point>>,
}

impl Cameras {
    fn from_handler<H>(handler: H) -> Self 
    where H:
    FnOnce(SyncSender<Option<Point>>) + Send + 'static,
    {
        let (send, recv) = sync_channel(1);

        std::thread::spawn(|| handler(send));

        Cameras { channel: recv }
    }

    /// Waits for the next scan to be available, and then returns the found point.
    pub fn next_scan(&mut self) -> Option<Point> {
        self.channel.recv().unwrap()
    }
}
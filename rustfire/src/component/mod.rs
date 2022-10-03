mod simulated;

use std::future::Future;
use tokio::sync::watch;

use crate::math::Point;

pub use self::simulated::simulated_lidar;

/// A lidar.
#[derive(Clone)]
pub struct Lidar {
    channel: watch::Receiver<Vec<Point>>,
}

impl Lidar {
    /// Creates a lidar with the specified handler, which should watch the lidar and
    /// update the value in the channel on each scan.
    fn from_handler<H, F>(handler: H) -> Self
    where
        H: FnOnce(watch::Sender<Vec<Point>>) -> F,
        F: Future<Output = ()> + Send + 'static,
    {
        let (send, recv) = watch::channel(Vec::new());

        tokio::spawn(handler(send));

        Lidar { channel: recv }
    }

    /// Waits for the next scan to be available, and then returns the scanned points.
    pub async fn next_scan(&mut self) -> Vec<Point> {
        let _ = self.channel.changed().await;
        self.channel.borrow().clone()
    }
}

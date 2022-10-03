use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};

use firefighter::math::{Line, Point, Radians, Transform, Vec2};
use firefighter::{simulated_lidar, Map};

#[tokio::main]
async fn main() {
    let map = Map {
        walls: vec![
            Line::new(Point::new(0.0, 2400.0), Point::new(700.0, 2400.0)),
            Line::new(Point::new(700.0, 2400.0), Point::new(2400.0, 2400.0)),
            Line::new(Point::new(2400.0, 2400.0), Point::new(2400.0, 900.0)),
            Line::new(Point::new(2400.0, 900.0), Point::new(2400.0, 0.0)),
            Line::new(Point::new(2400.0, 0.0), Point::new(1180.0, 0.0)),
            Line::new(Point::new(1180.0, 0.0), Point::new(0.0, 0.0)),
            Line::new(Point::new(0.0, 0.0), Point::new(0.0, 1020.0)),
            Line::new(Point::new(0.0, 1020.0), Point::new(0.0, 2400.0)),
            Line::new(Point::new(700.0, 2400.0), Point::new(700.0, 1490.0)),
            Line::new(Point::new(700.0, 1490.0), Point::new(470.0, 1490.0)),
            Line::new(Point::new(710.0, 470.0), Point::new(710.0, 1020.0)),
            Line::new(Point::new(710.0, 1020.0), Point::new(0.0, 1020.0)),
            Line::new(Point::new(1180.0, 0.0), Point::new(1180.0, 440.0)),
            Line::new(Point::new(1180.0, 900.0), Point::new(2400.0, 900.0)),
            Line::new(Point::new(1170.0, 1380.0), Point::new(1170.0, 1930.0)),
            Line::new(Point::new(1170.0, 1930.0), Point::new(1920.0, 1930.0)),
            Line::new(Point::new(1920.0, 1930.0), Point::new(1920.0, 1380.0)),
            Line::new(Point::new(1920.0, 1380.0), Point::new(1640.0, 1380.0)),
        ],
    };

    let start_pos = Transform::new(Radians(1.0), Vec2::new(1200.0, 1200.0));
    let position = Arc::new(Mutex::new(start_pos));

    tokio::spawn({
        let position = Arc::clone(&position);
        let start = Instant::now();
        async move {
            loop {
                let time = start.elapsed().as_secs_f32();
                let a1 = Radians(time * 0.1);
                let a2 = Radians(time * 1.0);

                let pos = Point::new(1200.0, 1200.0) + Vec2::new(800.0, 0.0).rotate(a1);

                *position.lock().unwrap() = Transform::new(a2, pos.vec2());

                tokio::time::sleep(Duration::from_millis(10)).await;
            }
        }
    });

    let mut distorted = map.clone();
    distorted.randomize(50.0);
    let lidar = simulated_lidar(distorted, position);

    firefighter::main(map, lidar).await;
}
use super::Lidar;
use crate::map::Map;
use crate::math::{Point, Radians, Ray, Transform, Vec2};
use std::sync::{Arc, Mutex};
use std::time::Duration;

/// A simulated lidar. Uses the value in the `Mutex` to determine the current
/// position each scan.
pub fn simulated_lidar(map: Map, position: Arc<Mutex<Transform>>) -> Lidar {
    // Lidar properties
    let scan_duration = Duration::from_millis(100);
    let min_angle = Radians::from_degrees(-120.0);
    let max_angle = Radians::from_degrees(120.0);
    let angular_resolution = 680;

    let mut interval = tokio::time::interval(scan_duration);

    Lidar::from_handler(|channel| async move {
        loop {
            interval.tick().await;

            let position = *position.lock().unwrap();

            let points = (0..angular_resolution)
                .filter_map(|i| {
                    let fraction = i as f32 / angular_resolution as f32;
                    let angle = Radians(min_angle.0 + (max_angle.0 - min_angle.0) * fraction);

                    let relative_direction = Vec2::new(0.0, -1.0).rotate(angle);
                    let ray = position * Ray::new(Point::ORIGIN, relative_direction);

                    let hit = map.raycast(ray)?;

                    // Too close to measure.
                    if (hit - ray.origin).length() < 150.0 {
                        return None;
                    }

                    Some(position.inverse() * hit)
                })
                .collect();

            if channel.send(points).is_err() {
                break;
            }
        }
    })
}

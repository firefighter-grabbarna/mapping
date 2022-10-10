use crate::display::Display;
use crate::math::{Radians, Transform, Vec2};
use crate::Lidar;

use super::Localizer;

/// Creates a `Localizer` which assumes the robot is stationary.
pub fn noop_localizer(mut lidar: Lidar, display: Option<Display>) -> Localizer {
    let position = Transform::new(Radians::from_degrees(0.0), Vec2::new(1300.0, 1300.0));
    Localizer::from_handler(move |channel| {
        loop {
            let mut points = lidar.next_scan();

            // Update the display.
            if let Some(ref display) = display {
                display.update_state(|state| {
                    // Apply the transform to the points.
                    for point in &mut points {
                        *point = position * *point;
                    }

                    state.points = points;
                    state.robot = position;
                    state.cost = 0.0;
                });
            }

            if channel.send(Some(position)).is_err() {
                break;
            }
        }
    })
}

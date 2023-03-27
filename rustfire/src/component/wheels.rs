use super::Serial;

/// Communication with the wheels.
pub struct Wheels {
    serial: Serial,
}

impl Wheels {
    /// Connects to the wheels.
    pub fn new(serial: Serial) -> Self {
        Wheels { serial }
    }

    /// Sets the speed in the directions. `1.0` is max speed.
    pub fn set_speed(&mut self, forward: f32, left: f32, counter_clockwise: f32) {
        let max_speed = 255.0;

        let scale = max_speed * 1.0_f32.max(forward.abs() + left.abs() + counter_clockwise.abs());

        let forward = (forward * scale) as i32;
        let left = (left * scale) as i32;
        let counter_clockwise = (counter_clockwise * scale) as i32;

        self.serial
            .output(&format!("{forward} {left} {counter_clockwise}"));
    }
}

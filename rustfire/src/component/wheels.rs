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
    pub fn set_speed(&mut self, forward: f32, right: f32, clockwise: f32) {
        let max_speed = 35.0;

        let scale = max_speed * 1.0_f32.max(forward.abs() + right.abs() + clockwise.abs());

        let forward = (forward * scale) as i32;
        let right = (right * scale) as i32;
        let clockwise = (clockwise * scale) as i32;

        //println!("{forward} {right} {clockwise}");

        self.serial
            .output(&format!("{forward} {right} {clockwise}"));
    }
}

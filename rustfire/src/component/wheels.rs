use std::sync::mpsc::{channel, RecvTimeoutError, Sender};
use std::time::Duration;

use super::Serial;

/// Communication with the wheels.
pub struct Wheels {
    channel: Sender<String>,
}

impl Wheels {
    /// Connects to the wheels.
    pub fn new(mut serial: Serial) -> Self {
        let (send, recv) = channel::<String>();

        std::thread::spawn(move || loop {
            match recv.recv_timeout(Duration::from_millis(250)) {
                Ok(msg) => serial.output(&msg),
                Err(RecvTimeoutError::Timeout) => serial.output("PING"),
                Err(RecvTimeoutError::Disconnected) => break,
            }
        });

        Wheels { channel: send }
    }

    /// Sets the speed in the directions. `1.0` is max speed.
    pub fn set_speed(&mut self, forward: f32, right: f32, rotate: f32, slow: bool) {
        let max_speed = 35.0;

        let scale = max_speed * 1.0_f32.max(forward.abs() + right.abs() + rotate.abs());

        let forward = (forward * scale) as i32;
        let right = (right * scale) as i32;
        let rotate = (rotate * scale) as i32;
        let slow = slow as i32;

        _ = self
            .channel
            .send(format!("{forward} {right} {rotate} {slow}"));
    }
}

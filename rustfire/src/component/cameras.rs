use super::Serial;

/// Communication with the
pub struct Cameras {
    serial: Serial,
}

impl Cameras {
    pub fn new(serial: Serial) -> Self {
        Cameras { serial }
    }

    pub fn search_for_light(&mut self) -> bool {
        self.serial.output("1");

        let response = self.serial.input();
        dbg!(&response);

        response == "found"
    }

    pub fn extinguish_fire(&mut self) {
        self.serial.output("3");

        let response = self.serial.input();
        dbg!(&response);
    }

    pub fn query(&mut self) {
        self.serial.output("4");

        let response = self.serial.input();
        dbg!(&response);
    }
}

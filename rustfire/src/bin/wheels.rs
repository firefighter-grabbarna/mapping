use firefighter::component::{Serial, Wheels};

fn main() {
    let serial = Serial::open("/dev/ttyACM0");

    let mut wheels = Wheels::new(serial);

    for line in std::io::stdin().lines() {
        let line = line.unwrap();

        let parts: Vec<f32> = line
            .split_whitespace()
            .map(|n| n.parse().unwrap())
            .collect();
        let forward = parts[0];
        let right = parts[1];
        let clockwise = parts[2];

        wheels.set_speed(forward, right, clockwise);
    }
}

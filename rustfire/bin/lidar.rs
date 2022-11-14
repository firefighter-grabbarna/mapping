use firefighter::component::{real_lidar, Serial};
use firefighter::math::Transform;
use firefighter::Display;

fn main() {
    let addr = "0.0.0.0:8000".parse().unwrap();
    let display = Display::listen(&addr);

    let serial = Serial::open("/dev/ttyACM0");
    let mut lidar = real_lidar(serial);

    display.update_state(|state| {
        state.view_box = [-1000.0, -1000.0, 2000.0, 2000.0];
        state.robot = Transform::IDENTITY;
    });

    loop {
        let points = lidar.next_scan();
        display.update_state(|state| {
            state.points = points;
        });
    }
}

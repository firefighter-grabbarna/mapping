use firefighter::component::{real_cameras, Serial};
use firefighter::math::Transform;
use firefighter::Display;

fn main() {
    let addr = "0.0.0.0:8000".parse().unwrap();
    let display = Display::listen(&addr);

    let serial = Serial::open("/dev/ttyACM0");
    let mut cameras = real_cameras(serial);

    display.update_state(|state| {
        state.view_box = [-1000.0, -1000.0, 2000.0, 2000.0];
        state.robot = Transform::IDENTITY;
    });

    loop {
        let _points = cameras.next_scan();
        display.update_state(|_state| {
            //state.points = points;
        });
    }
}

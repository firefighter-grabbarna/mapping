use firefighter::component::serial::Serial;
use firefighter::component::real_lidar;
use firefighter::display::Display;
use firefighter::math::Transform;

fn actual_main(display: Display) {
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


#[tokio::main]
async fn main() {
    let addr = "0.0.0.0:8000".parse().unwrap();
    let display = firefighter::display::listen(&addr);

    tokio::task::spawn_blocking(|| {
        actual_main(display);
    })
    .await
    .unwrap();
}

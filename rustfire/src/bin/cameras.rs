use firefighter::component::{Cameras, Serial};

fn main() {
    // let addr = "0.0.0.0:8000".parse().unwrap();
    // let display = Display::listen(&addr);

    let serial = Serial::open("/dev/ttyACM0");
    let mut cameras = Cameras::new(serial);

    // display.update_state(|state| {
    //     state.view_box = [-1000.0, -1000.0, 2000.0, 2000.0];
    //     state.robot = Transform::IDENTITY;
    // });

    println!("search for light");
    cameras.search_for_light();

    loop {
        std::thread::sleep(std::time::Duration::from_secs(1));
        //cameras.query();
    }

    // loop {
    //     // let _points = cameras.next_scan();
    //     display.update_state(|_state| {
    //         //state.points = points;
    //     });
    // }
}

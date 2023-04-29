use std::time::Duration;

use firefighter::component::{find_components, State};

fn main() -> std::io::Result<()> {
    let components = find_components();

    let mut cameras = components.cameras.expect("no cameras connected");

    cameras.set_state(State::Idle);

    loop {
        std::thread::sleep(Duration::from_millis(100));
        let (_, th_l, th_r) = cameras.get_state();

        if let (Some(th_l), Some(th_r)) = (th_l, th_r) {
            let average = (th_l.0 + th_r.0) / 2.0;
            let wedge = th_l.0 - th_r.0;

            println!("{} {}", average.to_degrees(), wedge.to_degrees());
        }

        // match cameras.get_state() {
        //     State::Search => continue,
        //     State::Idle => cameras.set_state(State::Search),
        //     State::Follow(_, _) => {
        //         std::thread::sleep(Duration::from_secs(2));

        //         match cameras.get_state() {
        //             State::Follow(angle1, angle2) => {
        //                 println!("{} {}", angle1, angle2);

        //                 let mut file = File::create("cal.txt")?;
        //                 writeln!(file, "{} {}", angle1, angle2)?;
        //                 return Ok(());
        //             }
        //             _ => continue,
        //         }
        //     }
        //     state => panic!("{state:?}"),
        // }
    }
}

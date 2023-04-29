use crate::math::Radians;

use super::Serial;

/// Communication with the
pub struct Cameras {
    // to_robot: Sender<State>,
    // from_robot: Arc<Mutex<State>>,
    serial: Serial,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum State {
    // // 8
    // Startup,
    // // 0
    // Idle,
    // // 1
    // Search,
    // // 2
    // Follow(i32, i32),
    // // 3
    // Query,
    // // 4
    // Extinguish,
    // // 6
    // Stop,
    // Unknown,

    // 0
    Idle,
    // 1
    WaitButton,
    // 2
    Stop,
    // 3
    Query,
    // 4
    Music,
    // 5
    Extinguish,
}

impl Cameras {
    pub fn new(serial: Serial) -> Self {
        // let (send1, recv1) = channel();
        // let send2 = Arc::new(Mutex::new(State::Startup));

        // let send3 = Arc::clone(&send2);
        // std::thread::spawn(move || loop {
        //     match recv1.try_recv() {
        //         Ok(state) => {
        //             let s = match state {
        //                 State::Startup => -1,
        //                 State::Idle => 0,
        //                 State::Search => 1,
        //                 State::Follow(_, _) => 2,
        //                 State::Extinguish => 3,
        //                 State::Stop => 6,
        //                 State::Unknown => 9,
        //             };
        //             serial.output(&format!("{s}"));
        //         },
        //         Err(TryRecvError::Empty) => {}
        //         Err(TryRecvError::Disconnected) => break,
        //     }

        //     let line = serial.input();
        //     let mut parts = line.split(',');

        //     let s: Option<i32> = parts.next().and_then(|n| n.parse().ok());
        //     parts.next();
        //     parts.next();
        //     parts.next();
        //     parts.next();
        //     let x: Option<i32> = parts.next().and_then(|n| n.parse().ok());
        //     let y: Option<i32> = parts.next().and_then(|n| n.parse().ok());

        //     let state = match (s, x, y) {
        //         (Some(-1), _, _) => State::Startup,
        //         (Some(0), _, _) => State::Idle,
        //         (Some(1), _, _) => State::Search,
        //         (Some(2), Some(x), Some(y)) => State::Follow(x, y),
        //         (Some(3), _, _) => State::Extinguish,
        //         (Some(6), _, _) => State::Stop,
        //         _ => State::Unknown,
        //     };

        //     if state == State::Stop {
        //         std::process::abort();
        //     }

        //     *send3.lock().unwrap() = state;
        // });

        //serial.discard_input();

        Cameras { serial }
    }

    pub fn set_state(&mut self, state: State) {
        // let s = match state {
        //     State::Startup => 8,
        //     State::Idle => 0,
        //     State::Search => 1,
        //     State::Follow(_, _) => 2,
        //     State::Extinguish => 3,
        //     State::Query => 4,
        //     State::Stop => 6,
        //     State::Unknown => 9,
        // };
        let s = match state {
            State::Idle => 0,
            State::WaitButton => 1,
            State::Stop => 2,
            State::Query => 3,
            State::Music => 4,
            State::Extinguish => 5,
        };
        // dbg!(s);
        self.serial.output(&format!("{s}"));
        // dbg!(self.serial.input()); // debug print
    }

    pub fn get_state(&mut self) -> (State, Option<Radians>, Option<Radians>) {
        self.set_state(State::Query);

        let line = self.serial.input();
        let parts: Vec<i32> = line.split(',').map(|n| n.parse().unwrap()).collect();

        let state = match parts[0] {
            0 => State::Idle,
            1 => State::WaitButton,
            2 => std::process::abort(),
            _ => panic!(),
        };

        let px_l = Some(parts[1]).filter(|&n| n != 1023);
        let px_r = Some(parts[3]).filter(|&n| n != 1023);

        let th_l = px_l.map(|px| Radians(0.577 - 7.36e-4 * px as f32));
        let th_r = px_r.map(|px| Radians(0.252 - 7.45e-4 * px as f32));

        println!("state: {:?}", (state, px_l, px_r, th_l, th_r));

        (state, th_l, th_r)
    }

    // pub fn search_for_light(&mut self) -> bool {
    //     self.to_robot.
    //     self.serial.output("1");

    //     let response = self.serial.input();
    //     dbg!(&response);

    //     response == "found"
    // }

    // pub fn extinguish_fire(&mut self) {
    //     self.serial.output("3");

    //     let response = self.serial.input();
    //     dbg!(&response);
    // }

    // pub fn query(&mut self) {
    //     self.serial.output("4");

    //     let response = self.serial.input();
    //     dbg!(&response);
    // }
}

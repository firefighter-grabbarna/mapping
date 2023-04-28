use super::Serial;

/// Communication with the
pub struct Cameras {
    // to_robot: Sender<State>,
    // from_robot: Arc<Mutex<State>>,
    serial: Serial,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum State {
    // -1
    Startup,
    // 0
    Idle,
    // 1
    Search,
    // 2
    Follow(i32, i32),
    // 3
    Query,
    // 4
    Extinguish,
    // 6
    Stop,

    Unknown,
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

        Cameras { serial }
    }

    pub fn set_state(&mut self, state: State) {
        let s = match state {
            State::Startup => -1,
            State::Idle => 0,
            State::Search => 1,
            State::Follow(_, _) => 2,
            State::Extinguish => 3,
            State::Query => 4,
            State::Stop => 6,
            State::Unknown => 9,
        };
        self.serial.output(&format!("{s}"));
    }

    pub fn get_state(&mut self) -> State {
        self.set_state(State::Query);

        let line = self.serial.input();
        let mut parts = line.split(',');

        let s: Option<i32> = parts.next().and_then(|n| n.parse().ok());
        parts.next();
        parts.next();
        parts.next();
        parts.next();
        let x: Option<i32> = parts.next().and_then(|n| n.parse().ok());
        let y: Option<i32> = parts.next().and_then(|n| n.parse().ok());

        let state = match (s, x, y) {
            (Some(-1), _, _) => State::Startup,
            (Some(0), _, _) => State::Idle,
            (Some(1), _, _) => State::Search,
            (Some(2), Some(x), Some(y)) => State::Follow(x, y),
            (Some(3), _, _) => State::Extinguish,
            (Some(4), _, _) => State::Query,
            (Some(6), _, _) => State::Stop,
            _ => State::Unknown,
        };

        dbg!(state);

        if state == State::Stop {
            std::process::abort();
        }

        state
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

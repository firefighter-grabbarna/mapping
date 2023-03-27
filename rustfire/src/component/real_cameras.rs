use std::sync::mpsc::{Receiver, channel};
use std::time::Duration;

use super::{Cameras, Serial};

fn read_thread() -> Receiver<String> {
    let (send, recv) = channel();
    std::thread::spawn(move || {
        loop {
            let mut line = String::new();
            std::io::stdin().read_line(&mut line).unwrap();
            send.send(line).unwrap();
        }
    });
    recv
}

pub fn real_cameras(mut serial: Serial) -> Cameras {
    Cameras::from_handler(move |_channel| {

        let input = read_thread();

        loop {
            //dbg!();
            if let Ok(line) = input.recv_timeout(Duration::from_millis(1)) {

                dbg!(&line);
                serial.output(line.trim());
            }

            if let Some(line) = serial.input_no_block() {
                dbg!(line);
            }

            // let mut line = String::new();
            // std::io::stdin().read_line(&mut line).unwrap();
            // serial.output(&line);

            // loop {
            //     let line = serial.input();
            //     dbg!(&line);
            //     if line.is_empty() {
            //         break;
            //     }
            // }

            // let line = serial.input();
            // dbg!(line);
        }

        // serial.output("1");
        // loop {
        //     let line = serial.input();

        //     let parts = line.split_whitespace();
        //     dbg!(parts);

        //     if channel.send(None).is_err() {
        //         break;
        //     }
        // }
    })
}

use std::io::{BufRead, BufReader, ErrorKind, Write};
use std::time::Duration;

use serial2::SerialPort;

pub struct Serial {
    port: BufReader<SerialPort>,
}

impl Serial {
    #[track_caller]
    pub fn open(path: &str) -> Self {
        let mut port = SerialPort::open(path, 115200).unwrap();
        port.set_read_timeout(Duration::from_millis(1000)).unwrap();
        port.set_write_timeout(Duration::from_millis(1000)).unwrap();

        std::thread::sleep(Duration::from_millis(3000));
        port.discard_input_buffer().unwrap();

        Serial {
            port: BufReader::new(port),
        }
    }
    #[track_caller]
    pub fn discard_input(&mut self) {
        self.port.get_mut().discard_buffers().unwrap();
    }
    #[track_caller]
    pub fn output(&mut self, value: &str) {
        // while let Ok(n) = self.port.read(&mut [0; 64]) {
        //     dbg!(n);
        // }
        // self.port.get_mut().discard_input_buffer().unwrap();

        writeln!(self.port.get_mut(), "{value}").unwrap();
        self.port.get_mut().flush().unwrap();
    }
    #[track_caller]
    pub fn input(&mut self) -> String {
        let mut buf = Vec::new();

        loop {
            match self.port.read_until(b'\n', &mut buf) {
                Ok(_) => break,
                Err(err) if err.kind() == ErrorKind::TimedOut => {
                    continue;
                }
                Err(err) => panic!("{err}"),
            }
        }
        while matches!(buf.last(), Some(b'\n' | b'\r')) {
            buf.pop();
        }
        String::from_utf8(buf).unwrap()
    }
    #[track_caller]
    pub fn input_no_block(&mut self) -> Option<String> {
        let mut buf = String::new();
        match self.port.read_line(&mut buf) {
            Ok(_) => {
                buf.pop();
                Some(buf)
            }
            Err(err) if err.kind() == ErrorKind::TimedOut => None,
            Err(err) => panic!("{err}"),
        }
    }
    #[track_caller]
    pub fn query(&mut self, query: &str) -> Vec<String> {
        self.output(query);
        let mut lines = Vec::new();
        loop {
            let line = self.input();
            if line.trim().is_empty() {
                return lines;
            }
            lines.push(line);
        }
    }
}

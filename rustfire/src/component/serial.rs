use std::io::{BufRead, BufReader, Write};
use std::time::Duration;

use serial2::SerialPort;

pub struct Serial {
    port: BufReader<SerialPort>,
}

impl Serial {
    pub fn open(path: &str) -> Self {
        let mut port = SerialPort::open(path, 115200).unwrap();
        port.set_read_timeout(Duration::from_secs(1)).unwrap();

        std::thread::sleep(Duration::from_millis(500));
        port.discard_input_buffer().unwrap();

        Serial {
            port: BufReader::new(port),
        }
    }
    fn output(&mut self, value: &str) {
        write!(self.port.get_mut(), "{value}\n").unwrap();
    }
    fn input(&mut self) -> String {
        let mut buf = String::new();
        self.port.read_line(&mut buf).unwrap();
        buf.pop();
        buf
    }
    pub fn query(&mut self, query: &str) -> Vec<String> {
        self.output(query);
        let mut lines = Vec::new();
        loop {
            let line = self.input();
            if line.is_empty() {
                return lines;
            }
            lines.push(line);
        }
    }
}

use serial2::SerialPort;

fn main() {
    for entry in std::fs::read_dir("/dev").unwrap() {
        let entry = entry.unwrap();
        let name = entry.file_name().to_string_lossy().into_owned();

        if name.starts_with("ttyAMA") || name.starts_with("ttyACM") || name.starts_with("ttyUSB") {
            _ = SerialPort::open(entry.path(), 115200);
        }
    }
}

use crate::component::Serial;

use super::{real_lidar, Cameras, Lidar, Wheels};

pub struct ComponentList {
    pub lidar: Option<Lidar>,
    pub wheels: Option<Wheels>,
    pub cameras: Option<Cameras>,
}

pub fn find_components() -> ComponentList {
    let mut components = ComponentList {
        lidar: None,
        wheels: None,
        cameras: None,
    };

    for entry in std::fs::read_dir("/dev").unwrap() {
        let entry = entry.unwrap();
        let name = entry.file_name().to_string_lossy().into_owned();

        if name.starts_with("ttyACM") || name.starts_with("ttyUSB") {
            let mut serial = Serial::open(&format!("/dev/{name}"));

            serial.output("SCIP2.0");
            match serial.input().as_str() {
                "sensor_arduino" => {
                    println!("cameras connected");
                    components.cameras = Some(Cameras::new(serial));
                }
                s if s.contains(',') => {
                    println!("cameras connected");
                    components.cameras = Some(Cameras::new(serial));
                }
                "MOTOR" => {
                    println!("wheels connected");
                    components.wheels = Some(Wheels::new(serial));
                }
                "SCIP2.0" => {
                    while !serial.input().is_empty() {}
                    println!("lidar connected");
                    components.lidar = Some(real_lidar(serial));
                }
                response => println!("unknown component: {response:?}"),
            }
        }
    }

    components
}

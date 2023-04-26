use super::{Lidar, Serial};
use crate::math::{Radians, Vec2};

struct Urg {
    serial: Serial,
    dmin: u32,
    dmax: u32,
    ares: u32,
    amin: u32,
    amax: u32,
    afrt: u32,
    _rpm: u32,
}

impl Urg {
    fn new(serial: Serial) -> Self {
        let mut lidar = Urg {
            serial,
            dmin: 20,
            dmax: 5600,
            ares: 1024,
            amin: 44,
            amax: 725,
            afrt: 384,
            _rpm: 600,
        };

        let res = lidar.serial.query("SCIP2.0");
        assert_eq!(res.len(), 2);
        assert_eq!(res[0], "SCIP2.0");

        // to do: query parameters

        let res = lidar.serial.query("BM");
        assert_eq!(res.len(), 2);
        assert_eq!(res[0], "BM");
        assert!(res[1] == "00P" || res[1] == "02R");

        lidar
    }
    fn scan(&mut self) -> Vec<i32> {
        let query = format!("GD{:04}{:04}01", self.amin, self.amax);
        let res = self.serial.query(&query);

        let data = res
            .iter()
            .skip(3)
            .flat_map(|s| &s.as_bytes()[..s.len() - 1])
            .collect::<Vec<_>>();

        let mut values = Vec::new();
        for bytes in data.chunks(3) {
            let mut v = 0;
            for &b in bytes {
                v = v * 64 + (b - b'0') as i32;
            }

            if v < self.dmin as i32 || v > self.dmax as i32 {
                v = -1;
            }
            values.push(v);
        }

        let right_rotate = self.ares / 2 + self.amin - self.afrt;

        values.resize(self.ares as usize, -1);
        values.rotate_right(right_rotate as usize);

        values
    }
}

/// Offset from the center of the robot to the lidar.
const LIDAR_OFFSET: Vec2 = Vec2::new(0.0, 75.0);

/// Connects to a real lidar at the specified serial port.
pub fn real_lidar(serial: Serial) -> Lidar {
    Lidar::from_handler(move |channel| {
        let mut lidar = Urg::new(serial);

        loop {
            let distances = lidar.scan();
            let mut points = Vec::new();

            for (i, &d) in distances.iter().enumerate() {
                if d > 150 {
                    let fraction = i as f32 / distances.len() as f32;
                    let angle = Radians(fraction * std::f32::consts::PI * 2.0);

                    let direction = Vec2::new(0.0, -1.0).rotate(angle);
                    points.push((direction * d as f32).point() + LIDAR_OFFSET);
                }
            }

            if channel.send(points).is_err() {
                break;
            }
        }
    })
}

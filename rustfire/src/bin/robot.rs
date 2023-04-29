// #![allow(dead_code)]

use std::time::Duration;

use firefighter::component::{find_components, Cameras, State, Wheels};
use firefighter::localizer::{icp_localizer, Localizer};
use firefighter::math::{Line, Point, Radians, Transform, Vec2};
use firefighter::path_finding::{
    NodeMap, PathPoint, ScanSettings, ALL_ROOMS_ORDER, ROOM_SEARCH_ORDER,
};
use firefighter::{Display, Map};

struct Robot {
    wheels: Wheels,
    cameras: Cameras,
    display: Display,
    localizer: Localizer,
    map: Map,
}

#[derive(Clone, Copy, PartialEq, Debug)]
enum RobotErr {
    Lost,
    Extinguished,
}

impl Robot {
    /// Drives in a straight line to the target. Avoids walls.
    fn drive_vector(&mut self, position: Transform, velocity: Vec2, rot_vel: f32) {
        const FORCE_START: f32 = 12.0;
        const FORCE_END: f32 = 20.0;

        let robot_pos = position.offset.point();
        let closest = self.map.point_closest_to(robot_pos);
        let wall_distance = (closest - robot_pos).length();
        let factor = (wall_distance - FORCE_END) / (FORCE_START - FORCE_END);
        let factor = 0.0; //factor.clamp(0.0, 1.0);

        let away_from_wall = (robot_pos - closest).normalize();
        let drive_velocity = velocity.limit(1.0);

        let velocity = drive_velocity + (away_from_wall - drive_velocity) * factor;

        let forward_hat = Vec2::new(0.0, 1.0).rotate(position.rotation);
        let right_hat = Vec2::new(1.0, 0.0).rotate(position.rotation);

        let forward = velocity.dot(forward_hat);
        let right = velocity.dot(right_hat);

        self.wheels.set_speed(forward, right, rot_vel);
    }

    /// Rotates to the angle.
    fn rotate_to(&mut self, angle: Radians) {
        const STOP_OFFSET: Radians = Radians(-60.0 * (std::f32::consts::PI / 180.0));

        self.wheels.set_speed(0.0, 0.0, 0.5);
        std::thread::sleep(Duration::from_secs(1));

        let start_angle = loop {
            if let Some(pos) = self.localizer.next_position() {
                break pos.rotation;
            }
        };

        let target = angle + STOP_OFFSET;

        let mut last_angle = start_angle;
        loop {
            let Some(pos) = self.localizer.next_position() else { continue };
            let new_angle = pos.rotation;

            let new_delta = (new_angle - target).wrapped();
            let last_delta = (last_angle - target).wrapped();
            if new_delta >= 0.0 && last_delta <= 0.0 {
                break;
            }

            last_angle = new_angle;
        }
        self.wheels.set_speed(0.0, 0.0, 0.0);
        std::thread::sleep(Duration::from_secs(1));
    }

    /// Drives along a line. Returns true if the end has been passed.
    fn drive_along(&mut self, position: Transform, line: Line) -> bool {
        let robot_pos = position.offset.point();

        // Get the closest point on the line.
        let closest = line.point_closest_to(robot_pos);

        // Get the unit vectors towards the line and towards the end.
        let towards_end = (line.p2 - robot_pos).normalize();
        let towards_line = (closest - robot_pos).normalize();

        // Determine how much the robot should steer towards the line.
        let dist_to_line = (closest - robot_pos).length();
        let factor = (dist_to_line / 200.0).min(1.0);

        let direction = towards_end + (towards_line - towards_end) * factor;
        let direction = direction.normalize() * 0.7;

        // Move in the determined direction.
        self.drive_vector(position, direction, 0.0);

        let line_unit_dir = (line.p2 - line.p1).normalize();
        let line_length = (line.p2 - line.p1).length();

        let end_point = line.p1 + line_unit_dir * (line_length - 150.0);
        (robot_pos - line.p1).dot(line_unit_dir) > (end_point - line.p1).dot(line_unit_dir)

        // let 

        // // Test if the robot has passed the end of the line.
        // (line.p2 - robot_pos).dot(line.p2 - line.p1) <= 0.0
    }

    /// Rotates until the current position is known.
    fn rotate_and_find_position(&mut self) -> Transform {
        if let Some(position) = self.localizer.next_position() {
            return position;
        }

        self.wheels.set_speed(0.0, 0.0, 0.5);

        let position = loop {
            if let Some(position) = self.localizer.next_position() {
                break position;
            }
        };

        self.wheels.set_speed(0.0, 0.0, 0.0);

        position
    }

    /// Spins around to find the fire. Faces the fire if it was found.
    fn spin_scan(&mut self, _target_angle: Radians) -> Result<bool, RobotErr> {

        for _ in 0..20 {

            self.wheels.set_speed(0.0, 0.0, 0.5);
            std::thread::sleep(Duration::from_millis(500));
            self.wheels.set_speed(0.0, 0.0, 0.0);
            std::thread::sleep(Duration::from_millis(500));


            let angle = match self.cameras.get_state() {
                (_, Some(a1), Some(a2)) => (a1.0 + a2.0) / 2.0,
                (_, Some(a1), None) => a1.0,
                (_, None, Some(a2)) => a2.0,
                _ => continue,
            };

            self.cameras.set_state(State::Music);

            return Ok(true);

            // self.localizer.next_position();
            // self.localizer.next_position();
            // self.localizer.next_position();
            // for _ in 0..10 {
            //     if let Some(_pos) = self.localizer.next_position() {
            //         //let target_angle = pos.rotation - Radians(angle);

            //         self.cameras.set_state(State::Music);

            //         return Ok(true);
            //     }
            // }


            // // See if we have passed the fire.
            // if let (_, Some(_th1), Some(_th2)) = self.cameras.get_state() {
                

            //     self.cameras.set_state(State::Music);

            //     self.localizer.next_position();
            //     return Ok(true);

            //     // if (th1.0 + th2.0) / 2.0 > 0.0 {
            //     //     // Fire passed, store the angle
            //     //     // To do: Magic constant?
            //     //     fire_angle = new_angle + STOP_OFFSET;
            //     //     break;
            //     // }
            // }


        }

        Ok(false)


        // const STOP_OFFSET: Radians = Radians(45.0 * (std::f32::consts::PI / 180.0));

        // // Rotate so the back faces the room
        // println!("rotating");
        // self.rotate_to(target_angle + Radians::from_degrees(180.0));

        // // Start rotating
        // println!("scanning");
        // self.wheels.set_speed(0.0, 0.0, 0.5);

        // let mut first_half = true;

        // let fire_angle;

        // loop {
        //     let Some(pos) = self.localizer.next_position() else { continue };
        //     let new_angle = pos.rotation;

        //     // Keep track of the full rotation
        //     let fw_delta = (new_angle - target_angle).wrapped();
        //     if first_half && fw_delta.abs() < 30_f32.to_radians() {
        //         first_half = false;
        //     }
        //     if !first_half && fw_delta.abs() > (180_f32 - 30_f32).to_radians() {
        //         // not found after a rotation, exiting
        //         self.wheels.set_speed(0.0, 0.0, 0.0);
        //         return Ok(false);
        //     }

        //     // See if we have passed the fire.
        //     if let (_, Some(th1), Some(th2)) = self.cameras.get_state() {
        //         if (th1.0 + th2.0) / 2.0 > 0.0 {
        //             // Fire passed, store the angle
        //             // To do: Magic constant?
        //             fire_angle = new_angle + STOP_OFFSET;
        //             break;
        //         }
        //     }
        // }

        // println!("rotating");
        // dbg!(fire_angle);
        // self.rotate_to(fire_angle);

        // self.cameras.set_state(State::Music);

        // Ok(true)
    }

    /// Drives slowly towards the fire.
    fn follow_and_extinguish(&mut self) -> Result<(), RobotErr> {
        const TURN_FACTOR: f32 = 1.0;
        const STOP_WEDGE: f32 = 35_f32 * (std::f32::consts::PI / 180.0);

        let found = loop {
            let (_, th_l, th_r) = self.cameras.get_state();

            match (th_l, th_r) {
                (None, None) => {
                    println!("fire lost");
                    break false;
                }
                (Some(angle), None) | (None, Some(angle)) => {
                    let rot_speed = if angle.0 > 0.0 { -0.5 } else { 0.5 };

                    self.wheels.set_speed(0.0, 0.0, rot_speed);
                }
                (Some(th_l), Some(th_r)) => {
                    let average = (th_l.0 + th_r.0) / 2.0;
                    let wedge = th_l.0 - th_r.0;

                    if wedge > STOP_WEDGE {
                        println!("stopped at fire");
                        break true;
                    }

                    let rot_speed = -average * TURN_FACTOR;
                    let fw_speed = 0.5;

                    println!("{}", wedge.to_degrees());

                    self.wheels.set_speed(fw_speed, 0.0, rot_speed);
                }
            }
        };

        self.wheels.set_speed(0.0, 0.0, 0.0);

        if !found {
            return Ok(());
        }

        // Drive forward
        std::thread::sleep(Duration::from_millis(1000));
        self.wheels.set_speed(0.5, 0.0, 0.0);
        std::thread::sleep(Duration::from_millis(650));
        self.wheels.set_speed(0.0, 0.0, 0.0);
        std::thread::sleep(Duration::from_millis(1000));

        self.cameras.set_state(State::Extinguish);
        _ = self.cameras.get_state();
        std::thread::sleep(Duration::from_millis(1000));

        Err(RobotErr::Extinguished)
    }

    fn follow_path(&mut self, path: Vec<PathPoint>) -> Result<(), RobotErr> {
        for window in path.windows(2) {
            let from = window[0];
            let to = window[1];

            println!("line from {:?} to {:?}", from.point, to.point);
            self.display
                .update_state(|state| state.target = Some(to.point));

            let line = Line::new(from.point, to.point);

            loop {
                let Some(position) = self.localizer.next_position() else {
                    self.display.update_state(|state| state.target = None);
                    self.wheels.set_speed(0.0, 0.0, 0.0);
                    std::thread::sleep(Duration::from_secs(1));
                    return Err(RobotErr::Lost);
                };

                if self.drive_along(position, line) {
                    break;
                }

                // // Ping the cameras to detect stop
                // self.cameras.get_state();
            }
            self.wheels.set_speed(0.0, 0.0, 0.0);

            if to.scan_settings == ScanSettings::Scan {
                let found = self.spin_scan(to.angle)?;
                if found {
                    self.follow_and_extinguish()?;
                }
            }
        }

        Ok(())
    }

    fn wait_for_button(&mut self) {
        self.cameras.set_state(State::WaitButton);
        while self.cameras.get_state().0 == State::WaitButton {
            self.localizer.next_position();
        }
        std::thread::sleep(Duration::from_secs(2));
    }

    fn run_route(&mut self) {
        // Create the map with the current position as starting position.
        let mut node_map;
        let start_room;
        loop {
            let position = self.rotate_and_find_position();

            node_map = NodeMap::new(position.offset.x, position.offset.y);
            if let Some(node) = node_map.get_current_node(node_map.start_x, node_map.start_y) {
                start_room = node;
                break;
            }

            self.wheels.set_speed(0.0, 0.0, 0.3);
            std::thread::sleep(Duration::from_secs(1));
        }

        println!("start room is {start_room:?}");

        let order = ROOM_SEARCH_ORDER
            .get(start_room as usize)
            .copied()
            .unwrap_or(ALL_ROOMS_ORDER);
        let mut target_idx = 0;

        loop {
            let position = self.rotate_and_find_position();
            let target = order[target_idx];

            println!("starting new path to {target:?}");

            let Some(path) = node_map.scan_room_path(position.offset.x, position.offset.y, target)
            else {
                self.wheels.set_speed(0.0, 0.0, 0.5);
                std::thread::sleep(Duration::from_secs(1));
                continue;
            };

            match self.follow_path(path) {
                Ok(()) => {
                    target_idx = (target_idx + 1) % order.len();
                    println!("path succeeded");
                }
                Err(RobotErr::Lost) => {
                    println!("path failed");
                }
                Err(RobotErr::Extinguished) => {
                    break;
                }
            }
        }

        loop {
            let position = self.rotate_and_find_position();
            let target = order[target_idx];

            println!("starting new path to {target:?}");

            let Some(path) = node_map.return_path(position.offset.x, position.offset.y)
            else {
                self.wheels.set_speed(0.0, 0.0, 0.5);
                std::thread::sleep(Duration::from_secs(1));
                continue;
            };

            match self.follow_path(path) {
                Ok(()) => return,
                Err(RobotErr::Lost) => continue,
                Err(RobotErr::Extinguished) => return,
            }
        }
    }
}

fn main() {
    let addr = "0.0.0.0:8000".parse().unwrap();
    let display = Display::listen(&addr);

    // Define the map
    let map = Map {
        walls: vec![
            Line::new(Point::new(0.0, 2400.0), Point::new(700.0, 2400.0)),
            Line::new(Point::new(700.0, 2400.0), Point::new(2400.0, 2400.0)),
            Line::new(Point::new(2400.0, 2400.0), Point::new(2400.0, 900.0)),
            Line::new(Point::new(2400.0, 900.0), Point::new(2400.0, 0.0)),
            Line::new(Point::new(2400.0, 0.0), Point::new(1180.0, 0.0)),
            Line::new(Point::new(1180.0, 0.0), Point::new(0.0, 0.0)),
            Line::new(Point::new(0.0, 0.0), Point::new(0.0, 1020.0)),
            Line::new(Point::new(0.0, 1020.0), Point::new(0.0, 2400.0)),
            Line::new(Point::new(700.0, 2400.0), Point::new(700.0, 1490.0)),
            Line::new(Point::new(700.0, 1490.0), Point::new(470.0, 1490.0)),
            Line::new(Point::new(710.0, 470.0), Point::new(710.0, 1020.0)),
            Line::new(Point::new(710.0, 1020.0), Point::new(0.0, 1020.0)),
            Line::new(Point::new(1180.0, 0.0), Point::new(1180.0, 440.0)),
            Line::new(Point::new(1180.0, 900.0), Point::new(2400.0, 900.0)),
            Line::new(Point::new(1170.0, 1380.0), Point::new(1170.0, 1930.0)),
            Line::new(Point::new(1170.0, 1930.0), Point::new(1920.0, 1930.0)),
            Line::new(Point::new(1920.0, 1930.0), Point::new(1920.0, 1380.0)),
            Line::new(Point::new(1920.0, 1380.0), Point::new(1640.0, 1380.0)),
        ],
    };

    display.update_state(|state| {
        state.view_box = [-200.0, -200.0, 2800.0, 2800.0];
        state.walls = map.walls.clone();
    });

    let components = find_components();

    let cameras = components.cameras.expect("no cameras connected");
    let lidar = components.lidar.expect("no lidar connected");
    let wheels = components.wheels.expect("no wheels connected");

    let localizer = icp_localizer(map.clone(), lidar, Some(display.clone()));

    let mut robot = Robot {
        wheels,
        cameras,
        display,
        localizer,
        map,
    };

    let run_program = true;

    loop {
        println!("Waiting");
        robot.wait_for_button();

        if run_program {
            robot.run_route();
        } else {
            let result = robot.spin_scan(Radians::from_degrees(-90.0));
            println!("spin: {result:?}");
            if result == Ok(true) {
                let result = robot.follow_and_extinguish();
                println!("extinguish: {result:?}");
            }

            robot.wheels.set_speed(0.0, 0.0, 0.0);
        }

        // _ = dbg!(result);

        // std::thread::sleep(Duration::from_secs(1_000_000_000));

        // robot.run_route();
        // if let Some(position) = localizer.next_position() {
        //     let target = Point::new(2100.0, 300.0);
        //     drive_towards(position, target, &mut wheels);
        // } else {
        //     wheels.set_speed(0.0, 0.0, 0.0);
        // }
    }
}

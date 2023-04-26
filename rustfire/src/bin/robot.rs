use firefighter::component::{find_components, Wheels};
use firefighter::localizer::{icp_localizer, Localizer};
use firefighter::math::{Line, Point, Transform, Vec2};
use firefighter::path_finding::{NodeMap, PathPoint, ROOM_SEARCH_ORDER};
use firefighter::{Display, Map};

struct Robot {
    wheels: Wheels,
    display: Display,
    localizer: Localizer,
    map: Map,
}

impl Robot {
    /// Drives in a straight line to the target. Avoids walls.
    fn drive_vector(&mut self, position: Transform, velocity: Vec2, slow: bool) {
        const FORCE_START: f32 = 12.0;
        const FORCE_END: f32 = 20.0;

        let robot_pos = position.offset.point();
        let closest = self.map.point_closest_to(robot_pos);
        let wall_distance = (closest - robot_pos).length();
        let factor = (wall_distance - FORCE_END) / (FORCE_START - FORCE_END);
        let factor = factor.clamp(0.0, 1.0);

        let away_from_wall = (robot_pos - closest).normalize();
        let drive_velocity = velocity.limit(1.0);

        let velocity = drive_velocity + (away_from_wall - drive_velocity) * factor;

        let forward_hat = Vec2::new(0.0, 1.0).rotate(position.rotation);
        let right_hat = Vec2::new(1.0, 0.0).rotate(position.rotation);

        let forward = velocity.dot(forward_hat);
        let right = velocity.dot(right_hat);

        self.wheels.set_speed(forward, right, 0.0, slow);
    }

    /// Drives along a line. Returns true if the end has been passed.
    fn drive_along(&mut self, position: Transform, line: Line) -> bool {
        #![allow(clippy::let_and_return)]

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
        let direction = direction.normalize();

        // Move in the determined direction.
        self.drive_vector(position, direction, false);

        // Test if the robot has passed the end of the line.
        (line.p2 - robot_pos).dot(line.p2 - line.p1) <= 0.0
    }

    fn follow_path(&mut self, path: Vec<PathPoint>) -> bool {
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
                    return false;
                };

                if self.drive_along(position, line) {
                    break;
                }
            }

            // to do: scan etc.
        }

        true
    }

    fn run_route(&mut self) {
        self.wheels.set_speed(0.0, 0.0, 0.0, false);
        let mut rotating = false;

        let mut node_map = loop {
            if let Some(point) = self.localizer.next_position() {
                break NodeMap::new(point.offset.x, point.offset.y);
            } else if !rotating {
                self.wheels.set_speed(0.0, 0.0, 0.3, true);
                rotating = true;
            }
        };

        self.wheels.set_speed(0.0, 0.0, 0.0, false);
        rotating = false;

        let start_room = node_map
            .get_current_node(node_map.start_x, node_map.start_y)
            .expect("unknown start room");

        println!("start room is {start_room:?}");

        let order = ROOM_SEARCH_ORDER[start_room as usize];
        let mut target_idx = 0;

        loop {
            let Some(position) = self.localizer.next_position() else {
                if !rotating {
                    self.wheels.set_speed(0.0, 0.0, 0.3, true);
                    rotating = true;
                }
                continue;
            };

            println!("starting new path");

            let path =
                node_map.scan_room_path(position.offset.x, position.offset.y, order[target_idx]);

            if self.follow_path(path) {
                target_idx = (target_idx + 1) % order.len();
                println!("path succeeded");
            } else {
                println!("path failed");
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

    // let cameras = components.cameras.expect("no cameras connected");
    let lidar = components.lidar.expect("no lidar connected");
    let wheels = components.wheels.expect("no wheels connected");

    let localizer = icp_localizer(map.clone(), lidar, Some(display.clone()));

    let mut robot = Robot {
        wheels,
        display,
        localizer,
        map,
    };

    loop {
        robot.run_route();
        // if let Some(position) = localizer.next_position() {
        //     let target = Point::new(2100.0, 300.0);
        //     drive_towards(position, target, &mut wheels);
        // } else {
        //     wheels.set_speed(0.0, 0.0, 0.0);
        // }
    }
}

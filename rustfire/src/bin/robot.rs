use firefighter::component::find_components;
use firefighter::localizer::icp_localizer;
use firefighter::math::{Line, Point, Vec2};
use firefighter::{Display, Map};

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
    let mut wheels = components.wheels.expect("no wheels connected");

    let mut localizer = icp_localizer(map, lidar, Some(display));

    loop {
        if let Some(position) = localizer.next_position() {
            let current = position.offset.point();
            let target = Point::new(2100.0, 300.0);

            //println!("{:?} {:?}", current, target);
            //println!("{:?}", Vec2::new(0.0, 1.0).rotate(position.rotation));

            let direction = target - current;
            let distance = direction.length();

            let speed = if distance < 50.0 {
                0.0
            } else {
                distance / 200.0
            };
            let target_velocity = direction * (speed / distance);

            let forward_hat = Vec2::new(0.0, 1.0).rotate(position.rotation);
            let right_hat = Vec2::new(1.0, 0.0).rotate(position.rotation);

            let forward = target_velocity.dot(forward_hat);
            let right = target_velocity.dot(right_hat);

            wheels.set_speed(forward, right, 0.0);
        } else {
            wheels.set_speed(0.0, 0.0, 0.0);
        }
    }

    // if let Some(ref mut cameras) = components.cameras {
    //     dbg!(cameras.search_for_light());
    // }
}

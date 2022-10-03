mod component;
mod display;
mod localizer;
mod map;
pub mod math;

use self::localizer::icp_localizer;

pub use self::component::{simulated_lidar, Lidar};
pub use self::map::Map;

pub async fn main(map: Map, lidar: Lidar) {
    let addr = "0.0.0.0:8000".parse().unwrap();
    let display = display::listen(&addr);

    display.update_state(|state| {
        state.view_box = [-200.0, -200.0, 2800.0, 2800.0];
        state.walls = map.walls.clone();
    });

    let mut localizer = icp_localizer(map, lidar, Some(display.clone()));

    loop {
        if let Some(_position) = localizer.next_position().await {
            // ...
        } else {
            // ...
        }
    }
}

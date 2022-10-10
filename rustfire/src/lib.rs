mod component;
pub mod display;
pub mod localizer;
mod map;
pub mod math;

use localizer::Localizer;

pub use self::component::{simulated_lidar, Lidar};
pub use self::map::Map;

pub fn main(mut localizer: Localizer) {
    // let mut localizer = localizer::noop_localizer(lidar, Some(display.clone()));

    loop {
        if let Some(_position) = localizer.next_position() {
            // ...
        } else {
            // ...
        }
    }
}

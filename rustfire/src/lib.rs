pub mod component;
mod display;
pub mod localizer;
mod map;
pub mod math;

use localizer::Localizer;

pub use self::map::Map;
pub use display::Display;

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

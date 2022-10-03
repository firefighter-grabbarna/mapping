use std::collections::HashMap;
use std::ops::RangeInclusive;

use ordered_float::OrderedFloat;
use rand::Rng;

use crate::math::{Line, Point, Ray, Vec2};

/// A set of walls.
#[derive(Clone, Debug)]
pub struct Map {
    /// The walls.
    pub walls: Vec<Line>,
}

impl Map {
    /// Finds the closest point where the ray intersects the walls.
    pub fn raycast(&self, ray: Ray) -> Option<Point> {
        self.walls
            .iter()
            .filter_map(|&line| ray.cast_onto(line))
            .min_by_key(|&point| OrderedFloat((point - ray.origin).length_sq()))
    }

    /// The point on the walls closest to the target point.
    pub fn point_closest_to(&self, target: Point) -> Point {
        self.walls
            .iter()
            .map(|&line| line.point_closest_to(target))
            .min_by_key(|&point| OrderedFloat((point - target).length_sq()))
            .expect("the map has no walls")
    }

    /// The coordinate range occupied by the walls.
    pub fn coord_range(&self) -> (RangeInclusive<f32>, RangeInclusive<f32>) {
        let points = self.walls.iter().flat_map(|wall| [wall.p1, wall.p2]);
        let ordered = |&v: &f32| OrderedFloat(v);

        let min_x = points.clone().map(|p| p.x).min_by_key(ordered).unwrap();
        let max_x = points.clone().map(|p| p.x).max_by_key(ordered).unwrap();
        let min_y = points.clone().map(|p| p.y).min_by_key(ordered).unwrap();
        let max_y = points.clone().map(|p| p.y).max_by_key(ordered).unwrap();

        (min_x..=max_x, min_y..=max_y)
    }

    /// Adds a random offset to each point.
    pub fn randomize(&mut self, amount: f32) {
        let mut cache = HashMap::new();
        let mut rng = rand::thread_rng();

        for line in &mut self.walls {
            for point in [&mut line.p1, &mut line.p2] {
                let key = (OrderedFloat(point.x), OrderedFloat(point.y));

                *point += *cache.entry(key).or_insert_with(|| {
                    let dx = rng.gen_range(-amount..=amount);
                    let dy = rng.gen_range(-amount..=amount);
                    Vec2::new(dx, dy).limit(amount)
                });
            }
        }
    }
}

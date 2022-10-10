use ordered_float::OrderedFloat;
use rand::Rng;

use crate::display::Display;
use crate::math::{Point, Radians, Transform, Vec2};
use crate::{Lidar, Map};

use super::{cost, Localizer};

/// Creates a `Localizer` using iterative closest point with the reference map
/// and lidar data.
pub fn icp_localizer(map: Map, mut lidar: Lidar, display: Option<Display>) -> Localizer {
    Localizer::from_handler(move |channel| {
        let mut last_pos = None;
        loop {
            let mut points = lidar.next_scan();

            let (new_pos, cost) = update_position(last_pos, &points, &map);

            // Update the display.
            if let Some(ref display) = display {
                display.update_state(|state| {
                    let pos = new_pos.unwrap_or(state.robot);

                    // Apply the transform to the points.
                    for point in &mut points {
                        *point = pos * *point;
                    }

                    state.points = points;
                    state.robot = pos;
                    state.cost = cost;
                });
            }

            last_pos = new_pos;
            if channel.send(new_pos).is_err() {
                break;
            }
        }
    })
}

fn update_position(
    last_pos: Option<Transform>,
    points: &[Point],
    map: &Map,
) -> (Option<Transform>, f32) {
    // Normal case, update the last position.
    if let Some(pos) = last_pos {
        let before = std::time::Instant::now();

        let pos = converge(pos, &points, &map);

        dbg!(before.elapsed());

        // Keep the result if it is good enough.
        let cost = cost(pos, &points, &map);
        if cost < 30.0 {
            return (Some(pos), cost);
        }
    }

    // Desync case, perform a full search.
    let pos = full_search(&points, &map);

    // Keep the result if it is good enough.
    let cost = cost(pos, &points, &map);
    if cost < 30.0 {
        return (Some(pos), cost);
    }

    (None, cost)
}

// Performs a single ICP step.
fn step(old_transform: Transform, points: &[Point], map: &Map) -> Transform {
    // Find a target for each point.
    let targets: Vec<Point> = points
        .iter()
        .map(|&p| old_transform * p)
        .map(|p| map.point_closest_to(p))
        .collect();

    // Find the average translation and the average target position.
    let mut translation_sum = Vec2::new(0.0, 0.0);
    let mut position_sum = Vec2::new(0.0, 0.0);
    for (&point, &target) in points.iter().zip(&targets) {
        translation_sum += target - point;
        position_sum += target.vec2();
    }

    let translation = translation_sum / points.len() as f32;
    let rotation_center = (position_sum / points.len() as f32).point();

    // Find the best rotation.
    // https://en.wikipedia.org/wiki/Procrustes_analysis#Rotation
    let mut num = 0.0;
    let mut den = 0.0;

    for (&point, &target) in points.iter().zip(&targets) {
        let p = point + translation - rotation_center;
        let t = target - rotation_center;

        num += p.cross(t);
        den += p.dot(t);
    }

    Transform::new(Radians(0.0), translation - rotation_center.vec2())
        .rotate(Radians(num.atan2(den)))
        .translate(rotation_center.vec2())
}

/// Performs ICP steps until it converges.
pub fn converge(mut guessed_pos: Transform, points: &[Point], map: &Map) -> Transform {
    const MAXIMUM_DIFF: f32 = 0.5;

    'outer: loop {
        let prev_transform = guessed_pos;

        guessed_pos = step(guessed_pos, points, map);

        for &point in points {
            if (prev_transform * point - guessed_pos * point).length_sq() > MAXIMUM_DIFF.powi(2) {
                continue 'outer;
            }
        }
        return guessed_pos;
    }
}

/// Searces many random positions to find a guess.
fn full_search(points: &[Point], map: &Map) -> Transform {
    // Range to generate random positions in.
    let (x_range, y_range) = map.coord_range();
    let angle_range = 0.0..2.0 * std::f32::consts::PI;
    let mut rng = rand::thread_rng();

    let mut random_pos = || {
        let a = rng.gen_range(angle_range.clone());
        let x = rng.gen_range(x_range.clone());
        let y = rng.gen_range(y_range.clone());
        Transform::new(Radians(a), Vec2::new(x, y))
    };

    let mut candidates: Vec<_> = (0..1024).map(|_| (random_pos(), 0.0)).collect();

    while candidates.len() > 4 {
        let step_size = (points.len() * 4 / candidates.len()).max(10);
        let points: Vec<_> = points.iter().copied().step_by(step_size).collect();

        for (cand_pos, cand_cost) in &mut candidates {
            for _ in 0..4 {
                *cand_pos = step(*cand_pos, &points, map);
            }
            *cand_cost = cost(*cand_pos, &points, map);
        }

        candidates.sort_unstable_by_key(|&(_, cost)| OrderedFloat(cost));
        candidates.truncate(candidates.len() / 2);
    }

    converge(candidates[0].0, points, map)
}

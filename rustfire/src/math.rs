use std::ops::{Add, AddAssign, Div, DivAssign, Mul, MulAssign, Neg, Sub, SubAssign};

use serde::Serialize;

/// A 2D vector.
#[derive(Clone, Copy, Debug, Serialize, Default)]
pub struct Vec2 {
    pub x: f32,
    pub y: f32,
}

impl Vec2 {
    pub const fn new(x: f32, y: f32) -> Self {
        Vec2 { x, y }
    }
    pub fn dot(self, rhs: Vec2) -> f32 {
        self.x * rhs.x + self.y * rhs.y
    }
    pub fn cross(self, rhs: Vec2) -> f32 {
        self.x * rhs.y - self.y * rhs.x
    }
    pub fn length(self) -> f32 {
        f32::hypot(self.x, self.y)
    }
    pub fn length_sq(self) -> f32 {
        self.dot(self)
    }
    pub fn normalize(self) -> Vec2 {
        self / self.length()
    }
    pub fn limit(self, length: f32) -> Vec2 {
        let factor = length / self.length().max(length);
        self * factor
    }
    pub fn rotate(self, angle: Radians) -> Vec2 {
        Vec2::new(
            self.x * angle.cos() - self.y * angle.sin(),
            self.y * angle.cos() + self.x * angle.sin(),
        )
    }
    pub fn point(self) -> Point {
        Point::new(self.x, self.y)
    }
}

impl Neg for Vec2 {
    type Output = Vec2;
    fn neg(self) -> Vec2 {
        Vec2::new(-self.x, -self.y)
    }
}
impl Add<Vec2> for Vec2 {
    type Output = Vec2;
    fn add(self, rhs: Vec2) -> Vec2 {
        Vec2::new(self.x + rhs.x, self.y + rhs.y)
    }
}
impl AddAssign<Vec2> for Vec2 {
    fn add_assign(&mut self, rhs: Vec2) {
        *self = *self + rhs;
    }
}
impl Sub<Vec2> for Vec2 {
    type Output = Vec2;
    fn sub(self, rhs: Vec2) -> Vec2 {
        Vec2::new(self.x - rhs.x, self.y - rhs.y)
    }
}
impl SubAssign<Vec2> for Vec2 {
    fn sub_assign(&mut self, rhs: Vec2) {
        *self = *self - rhs;
    }
}
impl Mul<f32> for Vec2 {
    type Output = Vec2;
    fn mul(self, rhs: f32) -> Vec2 {
        Vec2::new(self.x * rhs, self.y * rhs)
    }
}
impl MulAssign<f32> for Vec2 {
    fn mul_assign(&mut self, rhs: f32) {
        *self = *self * rhs;
    }
}
impl Mul<Vec2> for f32 {
    type Output = Vec2;
    fn mul(self: f32, rhs: Vec2) -> Vec2 {
        Vec2::new(self * rhs.x, self * rhs.y)
    }
}
impl Div<f32> for Vec2 {
    type Output = Vec2;
    fn div(self, rhs: f32) -> Vec2 {
        Vec2::new(self.x / rhs, self.y / rhs)
    }
}
impl DivAssign<f32> for Vec2 {
    fn div_assign(&mut self, rhs: f32) {
        *self = *self / rhs;
    }
}

/// A 2D point.
#[derive(Clone, Copy, Debug, Default, Serialize)]
pub struct Point {
    pub x: f32,
    pub y: f32,
}

impl Point {
    pub const ORIGIN: Point = Point::new(0.0, 0.0);
    pub const fn new(x: f32, y: f32) -> Self {
        Point { x, y }
    }
    pub fn vec2(self) -> Vec2 {
        Vec2::new(self.x, self.y)
    }
}

impl Add<Vec2> for Point {
    type Output = Point;
    fn add(self, rhs: Vec2) -> Point {
        Point::new(self.x + rhs.x, self.y + rhs.y)
    }
}
impl AddAssign<Vec2> for Point {
    fn add_assign(&mut self, rhs: Vec2) {
        *self = *self + rhs;
    }
}
impl Add<Point> for Vec2 {
    type Output = Point;
    fn add(self, rhs: Point) -> Point {
        Point::new(self.x + rhs.x, self.y + rhs.y)
    }
}
impl Sub<Vec2> for Point {
    type Output = Point;
    fn sub(self, rhs: Vec2) -> Point {
        Point::new(self.x - rhs.x, self.y - rhs.y)
    }
}
impl SubAssign<Vec2> for Point {
    fn sub_assign(&mut self, rhs: Vec2) {
        *self = *self - rhs;
    }
}
impl Sub<Point> for Point {
    type Output = Vec2;
    fn sub(self, rhs: Point) -> Vec2 {
        Vec2::new(self.x - rhs.x, self.y - rhs.y)
    }
}

/// A line segment.
#[derive(Clone, Copy, Debug, Serialize)]
pub struct Line {
    pub p1: Point,
    pub p2: Point,
}

impl Line {
    pub fn new(p1: Point, p2: Point) -> Self {
        Line { p1, p2 }
    }
    /// The closest point on the line to the target point.
    pub fn point_closest_to(self, target: Point) -> Point {
        let lv = self.p2 - self.p1;
        let pv = target - self.p1;

        let fraction = lv.dot(pv) / lv.dot(lv);

        if fraction <= 0.0 {
            self.p1
        } else if fraction >= 1.0 {
            self.p2
        } else {
            self.p1 + lv * fraction
        }
    }
}

/// An infinite ray.
#[derive(Clone, Copy, Debug)]
pub struct Ray {
    pub origin: Point,
    pub direction: Vec2,
}

impl Ray {
    pub fn new(origin: Point, direction: Vec2) -> Self {
        Ray { origin, direction }
    }
    /// Finds the intersection of `self` and `line`, if they cross.
    pub fn cast_onto(self, line: Line) -> Option<Point> {
        let v1 = self.direction;
        let v2 = line.p2 - line.p1;

        if v1.cross(v2) != 0.0 {
            let v3 = line.p1 - self.origin;

            let t1 = v3.cross(v2) / v1.cross(v2);
            let t2 = v3.cross(v1) / v1.cross(v2);

            if t1 >= 0.0 && (0.0..=1.0).contains(&t2) {
                return Some(self.origin + self.direction * t1);
            }
        }
        None
    }
}

/// An angle, in the range `-PI..=PI`.
#[derive(Clone, Copy, Debug, Serialize, Default)]
#[serde(transparent)]
pub struct Radians(pub f32);

impl Radians {
    pub fn from_degrees(degrees: f32) -> Self {
        Radians(degrees.to_radians())
    }
    pub fn wrapped(self) -> f32 {
        use std::f32::consts::PI;
        (self.0 + PI).rem_euclid(2.0 * PI) - PI
    }
    pub fn cos(self) -> f32 {
        self.0.cos()
    }
    pub fn sin(self) -> f32 {
        self.0.sin()
    }
}

impl Add<Radians> for Radians {
    type Output = Radians;
    fn add(self, rhs: Radians) -> Radians {
        Radians(self.0 + rhs.0)
    }
}
impl AddAssign<Radians> for Radians {
    fn add_assign(&mut self, rhs: Radians) {
        *self = *self + rhs;
    }
}
impl Sub<Radians> for Radians {
    type Output = Radians;
    fn sub(self, rhs: Radians) -> Radians {
        Radians(self.0 - rhs.0)
    }
}
impl SubAssign<Radians> for Radians {
    fn sub_assign(&mut self, rhs: Radians) {
        *self = *self - rhs;
    }
}
impl Neg for Radians {
    type Output = Radians;
    fn neg(self) -> Radians {
        Radians(-self.0)
    }
}

/// A rigid transform, represented as a rotation followed by a translation.
#[derive(Clone, Copy, Debug, Serialize, Default)]
pub struct Transform {
    pub rotation: Radians,
    pub offset: Vec2,
}

impl Transform {
    pub const IDENTITY: Self = Transform::new(Radians(0.0), Vec2::new(0.0, 0.0));

    pub const fn new(rotation: Radians, offset: Vec2) -> Self {
        Transform { rotation, offset }
    }

    pub fn inverse(self) -> Transform {
        Transform {
            rotation: -self.rotation,
            offset: -self.offset.rotate(-self.rotation),
        }
    }

    pub fn rotate(self, amount: Radians) -> Transform {
        Transform {
            rotation: self.rotation + amount,
            offset: self.offset.rotate(amount),
        }
    }

    pub fn translate(self, amount: Vec2) -> Transform {
        Transform {
            rotation: self.rotation,
            offset: self.offset + amount,
        }
    }
}

impl Mul<Vec2> for Transform {
    type Output = Vec2;
    fn mul(self, rhs: Vec2) -> Vec2 {
        rhs.rotate(self.rotation)
    }
}
impl Mul<Point> for Transform {
    type Output = Point;
    fn mul(self, rhs: Point) -> Point {
        #![allow(clippy::suspicious_arithmetic_impl)]
        rhs.vec2().rotate(self.rotation).point() + self.offset
    }
}
impl Mul<Line> for Transform {
    type Output = Line;
    fn mul(self, rhs: Line) -> Line {
        Line::new(self * rhs.p1, self * rhs.p2)
    }
}
impl Mul<Ray> for Transform {
    type Output = Ray;
    fn mul(self, rhs: Ray) -> Ray {
        Ray::new(self * rhs.origin, self * rhs.direction)
    }
}

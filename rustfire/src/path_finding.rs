#![allow(clippy::all)]

use std::collections::HashMap;

use queue::*;

use crate::math::{Point, Radians};

pub const ROOM_SEARCH_ORDER: &[&[NodeIndex]] = &[
    &[NodeIndex::S3, NodeIndex::S2, NodeIndex::S1, NodeIndex::S0], //room0
    &[NodeIndex::S2, NodeIndex::S3, NodeIndex::S0, NodeIndex::S1], //room1
    &[NodeIndex::S1, NodeIndex::S3, NodeIndex::S0, NodeIndex::S2], //room2
    &[NodeIndex::S0, NodeIndex::S2, NodeIndex::S1, NodeIndex::S3], //room3
];
pub const ALL_ROOMS_ORDER: &[NodeIndex] =
    &[NodeIndex::S2, NodeIndex::S1, NodeIndex::S3, NodeIndex::S0];

#[derive(Clone, Copy, PartialEq, Eq, Hash, Debug)]
pub enum NodeIndex {
    S0,
    S1,
    S2,
    S3,
    C0,
    C1,
    C2,
    C3,
    C4,
    C5,
    C6,
    C7,
}

#[derive(Clone, Copy, PartialEq)]
pub enum ScanSettings {
    NoScan,
    Scan,
    Done,
}

#[derive(Clone, Copy)]
pub struct PathPoint {
    pub point: Point,
    pub scan_settings: ScanSettings,
    pub angle: Radians,
}

impl PathPoint {
    fn new(point: Point, scan_settings: ScanSettings, angle: Radians) -> PathPoint {
        return PathPoint {
            point,
            scan_settings,
            angle,
        };
    }
}

pub struct Node {
    pub node_point: Point,
    pub bound_low_left: Point,
    pub bound_upp_right: Point,
    pub neighbours: Vec<NodeIndex>,
    pub previous: Option<NodeIndex>,
    pub index: NodeIndex,
    pub visited: bool,
    pub scan_angle: Radians,
}

impl Node {
    pub fn new(
        index: NodeIndex,
        node_point: Point,
        bound_low_left: Point,
        bound_upp_right: Point,
        scan_angle: Radians,
        neighbours: Vec<NodeIndex>,
    ) -> Node {
        return Node {
            node_point,
            bound_low_left,
            bound_upp_right,
            index,
            neighbours,
            previous: None,
            visited: false,
            scan_angle,
        };
    }

    pub fn get_neighbours(&self) -> Vec<NodeIndex> {
        return self.neighbours.clone();
    }

    pub fn add_neighbours(&mut self, index: NodeIndex) {
        self.neighbours.push(index);
    }

    pub fn set_neighbours(&mut self, neighbours: Vec<NodeIndex>) {
        self.neighbours = neighbours;
    }

    pub fn is_visited(&self) -> bool {
        return self.visited;
    }

    pub fn set_visited(&mut self, val: bool) {
        self.visited = val;
    }

    pub fn set_previous(&mut self, previous: NodeIndex) {
        self.previous = Some(previous);
    }

    pub fn in_bounds(&self, x: f32, y: f32) -> bool {
        // println!(
        //     "{:?}, {} {} {} {}",
        //     self.index,
        //     self.bound_low_left.x <= x,
        //     x <= self.bound_upp_right.x,
        //     self.bound_low_left.y <= y,
        //     y <= self.bound_low_left.y
        // );
        return self.bound_low_left.x <= x
            && x <= self.bound_upp_right.x
            && self.bound_low_left.y <= y
            && y <= self.bound_upp_right.y;
    }
}

pub struct NodeMap {
    pub nodes: HashMap<NodeIndex, Node>,
    pub start_x: f32,
    pub start_y: f32,
}

impl NodeMap {
    pub fn new(x: f32, y: f32) -> NodeMap {
        let mut map = NodeMap {
            nodes: HashMap::new(),
            start_x: x,
            start_y: y,
        };
        map.init();
        return map;
    }

    fn init(&mut self) {
        let nodes: Vec<Node> = vec![
            Node::new(
                NodeIndex::S0,
                Point::new(510.0, 235.0),
                Point::new(0.0, 0.0),
                Point::new(710.0, 1020.0),
                Radians::from_degrees(0.0),
                vec![NodeIndex::C0],
            ),
            Node::new(
                NodeIndex::S1,
                Point::new(235.0, 1690.0),
                Point::new(0.0, 1490.0),
                Point::new(700.0, 2400.0),
                Radians::from_degrees(-45.0),
                vec![NodeIndex::C1],
            ),
            Node::new(
                NodeIndex::S2,
                Point::new(1405.0, 1580.0),
                Point::new(1170.0, 1380.0),
                Point::new(1920.0, 1930.0),
                Radians::from_degrees(-80.0),
                vec![NodeIndex::C2],
            ),
            Node::new(
                NodeIndex::S3,
                Point::new(1380.0, 670.0),
                Point::new(1180.0, 0.0),
                Point::new(2400.0, 900.0),
                Radians::from_degrees(-135.0),
                vec![NodeIndex::C3],
            ),
            Node::new(
                NodeIndex::C0,
                Point::new(945.0, 235.0),
                Point::new(710.0, 0.0),
                Point::new(1180.0, 440.0),
                Radians::from_degrees(90.0),
                vec![NodeIndex::S0, NodeIndex::C3],
            ),
            Node::new(
                NodeIndex::C1,
                Point::new(235.0, 1255.0),
                Point::new(0.0, 1020.0),
                Point::new(710.0, 1490.0),
                Radians::from_degrees(0.0),
                vec![NodeIndex::S1, NodeIndex::C4],
            ),
            Node::new(
                NodeIndex::C2,
                Point::new(1405.0, 1140.0),
                Point::new(1170.0, 900.0),
                Point::new(1920.0, 1380.0),
                Radians::from_degrees(0.0),
                vec![NodeIndex::S2, NodeIndex::C4, NodeIndex::C7],
            ),
            Node::new(
                NodeIndex::C3,
                Point::new(945.0, 670.0),
                Point::new(710.0, 440.0),
                Point::new(1180.0, 900.0),
                Radians::from_degrees(-90.0),
                vec![NodeIndex::S3, NodeIndex::C0, NodeIndex::C4],
            ),
            Node::new(
                NodeIndex::C4,
                Point::new(945.0, 1255.0),
                Point::new(710.0, 900.0),
                Point::new(1170.0, 1490.0),
                Radians::default(),
                vec![NodeIndex::C1, NodeIndex::C2, NodeIndex::C3, NodeIndex::C5],
            ),
            Node::new(
                NodeIndex::C5,
                Point::new(945.0, 2165.0),
                Point::new(700.0, 1490.0),
                Point::new(1170.0, 2400.0),
                Radians::default(),
                vec![NodeIndex::C4, NodeIndex::C6],
            ),
            Node::new(
                NodeIndex::C6,
                Point::new(2160.0, 2165.0),
                Point::new(1170.0, 1930.0),
                Point::new(2400.0, 2400.0),
                Radians::default(),
                vec![NodeIndex::C5],
            ),
            Node::new(
                NodeIndex::C7,
                Point::new(2160.0, 1140.0),
                Point::new(1920.0, 900.0),
                Point::new(2400.0, 1930.0),
                Radians::default(),
                vec![NodeIndex::C2],
            ),
        ];

        for node in nodes {
            self.nodes.insert(node.index, node);
        }
    }

    pub fn get_current_node(&self, x: f32, y: f32) -> Option<NodeIndex> {
        for (&idx, node) in &self.nodes {
            if node.in_bounds(x, y) {
                return Some(idx);
            }
        }
        return None;
    }

    pub fn get_path(&mut self, start_node: NodeIndex, end_node: NodeIndex) -> Vec<NodeIndex> {
        let mut processing_queue: Queue<NodeIndex> = Queue::new();
        let mut path: Vec<NodeIndex> = vec![];

        // Reset the previous nodes.
        self.reset_visited();

        let _ = processing_queue.queue(start_node);
        self.nodes.get_mut(&start_node).unwrap().visited = true;

        println!("start_node = {:?}, end_node = {:?}", start_node, end_node);
        while !processing_queue.is_empty() {
            let current_node = processing_queue.dequeue().unwrap();
            if current_node == end_node {
                self.recreate_path(start_node, end_node, &mut path);
                return path;
            } else {
                for i in 0..self.nodes[&current_node].neighbours.len() {
                    let neighbours = self.nodes[&current_node].neighbours[i];
                    let neighbor_node = self.nodes.get_mut(&neighbours).unwrap();
                    if !neighbor_node.visited {
                        neighbor_node.visited = true;
                        neighbor_node.previous = Some(current_node);
                        let _ = processing_queue.queue(neighbours);
                    }
                }
            }
        }
        panic!("no path to node");
    }

    pub fn reset_visited(&mut self) {
        for node in self.nodes.values_mut() {
            node.visited = false;
        }
    }

    pub fn recreate_path(
        &mut self,
        start_node: NodeIndex,
        end_node: NodeIndex,
        path: &mut Vec<NodeIndex>,
    ) {
        let mut backward_path: Vec<NodeIndex> = vec![];
        let mut current_node: NodeIndex = end_node;

        while current_node != start_node {
            backward_path.push(current_node);
            current_node = self.nodes[&current_node].previous.unwrap();
        }
        backward_path.push(current_node);

        for i in (0..backward_path.len()).rev() {
            path.push(backward_path[i]);
        }
    }

    pub fn scan_room_path(&mut self, x: f32, y: f32, room: NodeIndex) -> Option<Vec<PathPoint>> {
        let current_node = self.get_current_node(x, y)?;
        let path = self.get_path(current_node, room);

        println!("scan path with {path:?}");

        let mut points: Vec<PathPoint> = path
            .iter()
            .map(|node_idx| {
                let node = &self.nodes[node_idx];
                PathPoint::new(node.node_point, ScanSettings::NoScan, node.scan_angle)
            })
            .collect();

        let len = points.len();
        if points.len() > 1 {
            points[len - 2].scan_settings = ScanSettings::Scan;
        }
        points[len - 1].scan_settings = ScanSettings::Scan;

        Some(points)
    }

    pub fn return_path(&mut self, x: f32, y: f32) -> Option<Vec<PathPoint>> {
        let current_node = self.get_current_node(x, y)?;
        let start_room = self.get_current_node(self.start_x, self.start_y).unwrap();
        let path = self.get_path(current_node, start_room);

        println!("return path with {path:?}");

        let mut points: Vec<PathPoint> = path
            .iter()
            .map(|node_idx| {
                let node = &self.nodes[node_idx];
                PathPoint::new(node.node_point, ScanSettings::NoScan, node.scan_angle)
            })
            .collect();

        points.push(PathPoint::new(
            Point::new(self.start_x, self.start_y),
            ScanSettings::Done,
            Radians::default(),
        ));

        Some(points)
    }

    // pub fn go_home(&mut self) {
    //     let current_x = self.global_path[self.cur_path_index as usize].x;
    //     let current_y = self.global_path[self.cur_path_index as usize].y;
    //     let mut current_node_index: usize = 0;
    //     let mut end_node_index: usize = 0;

    //     self.global_path = vec![];
    //     self.cur_path_index = 0;

    //     for node_index in 0..self.nodes.len() {
    //         if self.nodes[node_index].in_bounds(current_x, current_y) {
    //             current_node_index = node_index;
    //         }
    //     }

    //     for node_index in 0..self.nodes.len() {
    //         if self.nodes[node_index].in_bounds(self.start_x, self.start_y) {
    //             end_node_index = node_index;
    //         }
    //     }

    //     let home_path: Vec<usize> = self.get_path(current_node_index, end_node_index);
    //     println!(
    //         "the lenght of the path is {} from {} to {}",
    //         home_path.len(),
    //         current_node_index,
    //         end_node_index
    //     );
    //     for current_node in home_path {
    //         println!("{:?}", self.nodes[current_node].name);
    //         // If the node should be scanned and it has not been scanned, then it should be scanned.
    //         self.global_path.push(PathPoint::new(
    //             self.nodes[current_node].node_point.x,
    //             self.nodes[current_node].node_point.y,
    //             ScanSettings::NoScan,
    //             self.nodes[current_node].scan_angle,
    //         ))
    //     }
    // }

    // pub fn create_global_path(&mut self) {
    //     self.global_path = vec![];
    //     let mut current_path: Vec<usize> = vec![];

    //     let start_room: usize = self.get_current_node(self.start_x, self.start_y).unwrap();
    //     let mut previous_visited_room: usize = start_room;
    //     let mut scan_settings: ScanSettings;

    //     //Go throught all the rooms and find the paths to them from the previous visited room.
    //     for room_index in 0..AMOUNT_OF_ROOMS {
    //         // println!("start_node: {}, end_node:{}", previous_visited_room, ROOM_SEARCH_ORDER[start_room][room_index]);
    //         current_path = self.get_path(
    //             previous_visited_room,
    //             ROOM_SEARCH_ORDER[start_room][room_index],
    //         );

    //         // for i in 0..current_path.len() {
    //         // println!("{:?}", self.nodes[current_path[i]].name);
    //         // }
    //         for current_node in current_path {
    //             // If the node should be scanned and it has not been scanned, then it should be scanned.
    //             if self.nodes[current_node].scan_settings == ScanSettings::Scan
    //                 && !self.nodes[current_node].scanned
    //             {
    //                 scan_settings = ScanSettings::Scan;
    //             } else {
    //                 scan_settings = ScanSettings::NoScan;
    //             }
    //             self.nodes[current_node].scanned = true;

    //             self.global_path.push(PathPoint::new(
    //                 self.nodes[current_node].node_point.x,
    //                 self.nodes[current_node].node_point.y,
    //                 scan_settings,
    //                 self.nodes[current_node].scan_angle,
    //             ))
    //         }
    //         previous_visited_room = ROOM_SEARCH_ORDER[start_room][room_index]
    //     }

    //     let last_index = self.global_path.len() - 1;
    //     self.global_path[last_index].scan_settings = ScanSettings::Done;
    // }

    // pub fn get_next_path(&mut self) -> &PathPoint {
    //     let ret = &self.global_path[self.cur_path_index as usize];
    //     self.cur_path_index += 1;
    //     return ret;
    // }
}
// fn main() {
//     let mut node_map: NodeMap = NodeMap::new(1191, 1491);
//     node_map.create_global_path();
//     let mut path_point: &PathPoint;
//     for i in 0..node_map.global_path.len() {
//         path_point = node_map.get_next_path();
//         println!("{} {}", path_point.x, path_point.y);
//         if i == 10 {
//             println!("===========================");
//             println!("Going home :)");
//             println!("===========================");
//             node_map.go_home();
//             break;
//         }
//     }
//     println!("===========================");
//     println!("Going home :)");
//     println!("===========================");
//     for i in 0..node_map.global_path.len() {
//         path_point = node_map.get_next_path();
//         println!("{} {}", path_point.x, path_point.y);
//     }
// }

//oscar was here

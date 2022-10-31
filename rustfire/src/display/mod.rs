use std::net::SocketAddr;
use std::sync::{Arc, Mutex};
use std::time::Duration;

use serde::Serialize;

use crate::math::{Line, Point, Transform};

pub mod http_ws;

/// A handle to the display. The server is closed when dropped.
#[derive(Clone)]
pub struct Display {
    inner: Arc<Mutex<Inner>>,
}

struct Inner {
    server: http_ws::Server<State>,
    state: State,
}

impl Display {
    /// Updates the display state with the specified closure.
    pub fn update_state<R>(&self, f: impl FnOnce(&mut State) -> R) -> R {
        let mut inner = self.inner.lock().unwrap();
        f(&mut inner.state)
    }
}

/// The state to send to the client.
#[derive(Serialize, Default, Debug)]
pub struct State {
    pub view_box: [f32; 4],
    pub points: Vec<Point>,
    pub walls: Vec<Line>,
    pub robot: Transform,
    pub cost: f32,
}

/// Listens on the specified address, returning a handle to update the data to
/// display.
pub fn listen(addr: &SocketAddr) -> Display {
    let server = http_ws::listen(addr, |url| match url {
        "/" => Some((
            &include_bytes!("../../display/static/index.html")[..],
            "text/html;charset=UTF-8",
        )),
        "/index.js" => Some((
            &include_bytes!("../../display/static/index.js")[..],
            "text/javascript;charset=UTF-8",
        )),
        "/favicon.ico" => Some((&[], "image/x-icon")),
        _ => None,
    });

    let state = State::default();
    let inner = Arc::new(Mutex::new(Inner { server, state }));

    // Spawn a task which sends the state every 100ms (if changed).
    let inner2 = Arc::downgrade(&inner);
    std::thread::spawn(move || loop {
        if let Some(inner) = inner2.upgrade() {
            let mut inner = inner.lock().unwrap();
            let Inner { server, state } = &mut *inner;
            server.set_state(state);
        } else {
            break;
        }
        std::thread::sleep(Duration::from_millis(100));
    });

    Display { inner }
}

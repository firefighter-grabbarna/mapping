//! Simple HTTP server, which also sends the state to all connected sockets when
//! it changes.

use std::convert::Infallible;
use std::marker::PhantomData;
use std::net::SocketAddr;
use std::sync::Arc;

use futures_util::{SinkExt, StreamExt};
use hyper::{Body, Request, Response};
use hyper_tungstenite::HyperWebsocket;
use serde::Serialize;
use tokio::sync::watch::{channel, Receiver, Sender};
use tungstenite::error::ProtocolError;
use tungstenite::{Message, Result as WsResult};

/// A handle to the server, which allows changing the state.
/// The server is closed when this is dropped.
pub struct Server<S: ?Sized> {
    state_send: Sender<Option<Vec<u8>>>,
    phantom: PhantomData<S>,
}

impl<S: Serialize + ?Sized> Server<S> {
    /// Changes the current state, resending to all clients if changed.
    pub fn set_state(&mut self, state: &S) {
        // Serialize the state and send if changed.
        let data = Some(rmp_serde::to_vec_named(state).unwrap());
        if data != *self.state_send.borrow() {
            self.state_send.send(data).unwrap();
        }
    }
}

/// Listens on the specified address, calling the handle for each GET request.
pub fn listen<S, H, B>(addr: &SocketAddr, handler: H) -> Server<S>
where
    S: Serialize + ?Sized,
    H: Fn(&str) -> Option<(B, &'static str)> + Send + Sync + 'static,
    B: Into<Body> + 'static,
{
    let (state_send, state_recv) = channel(None);

    let handler = Arc::new(handler);

    // A future which resolves when the state channel is closed.
    let wait_until_dropped = {
        let mut state_recv = state_recv.clone();
        async move { while state_recv.changed().await.is_ok() {} }
    };

    let make_service = hyper::service::make_service_fn(move |_socket| {
        let handler = Arc::clone(&handler);
        let state_recv = state_recv.clone();

        async move {
            let service_fn = hyper::service::service_fn(move |req| {
                let handler = Arc::clone(&handler);
                let state_recv = state_recv.clone();

                handle_request(req, handler, state_recv)
            });
            Ok::<_, Infallible>(service_fn)
        }
    });

    tokio::spawn(
        hyper::Server::bind(addr)
            .serve(make_service)
            .with_graceful_shutdown(wait_until_dropped),
    );

    Server {
        state_send,
        phantom: PhantomData,
    }
}

/// Handles a single request.
async fn handle_request<H, B>(
    req: Request<Body>,
    handler: Arc<H>,
    state_recv: Receiver<Option<Vec<u8>>>,
) -> Result<Response<Body>, ProtocolError>
where
    H: Fn(&str) -> Option<(B, &'static str)>,
    B: Into<Body>,
{
    if hyper_tungstenite::is_upgrade_request(&req) {
        let (res, socket) = hyper_tungstenite::upgrade(req, None)?;

        tokio::spawn(async {
            if let Err(err) = handle_socket(socket, state_recv).await {
                eprintln!("WebSocket error: {err}");
            }
        });

        Ok(res)
    } else {
        let (body, mime, status) = match handler(req.uri().path()) {
            Some((body, mime)) => (body.into(), mime, 200),
            None => ("Error 404".into(), "text/plain", 404),
        };
        Ok(Response::builder()
            .status(status)
            .header("Content-Type", mime)
            .header("Cache-Control", "max-age=0")
            .body(body)
            .unwrap())
    }
}

/// Handles a socket connection.
async fn handle_socket(
    socket: HyperWebsocket,
    mut state_recv: Receiver<Option<Vec<u8>>>,
) -> WsResult<()> {
    let socket = socket.await?;

    let (mut socket_out, mut socket_in) = socket.split();

    // Listens for messages and resolves when the socket is closed.
    let mut wait_for_close = tokio::spawn(async move {
        loop {
            match socket_in.next().await {
                Some(Ok(_message)) => continue,
                Some(Err(err)) => break Err(err),
                None => break Ok(()),
            }
        }
    });

    loop {
        tokio::select! {
            result = &mut wait_for_close => {
                // Break if the socket has closed.
                return result.unwrap();
            }
            result = state_recv.changed() => match result {
                Ok(()) => {
                    // Send the updated state to the socket.
                    let data = (*state_recv.borrow()).clone();
                    if let Some(data) = data {
                        socket_out.send(Message::Binary(data)).await?;
                    }
                }
                Err(_) => {
                    // The channel was closed, close the socket.
                    socket_out.send(Message::Close(None)).await?;
                    return wait_for_close.await.unwrap();
                }
            }
        }
    }
}

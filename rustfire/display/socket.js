import { parse } from "./msgpack";

/// Connects to the server, calling the handler when the state changes. Calls
/// the handler with `null` when the connection is dropped. Automatically
/// reloads the page to reconnect when the server is up.
export function connect(handler) {
    let ws = new WebSocket(`ws://${location.host}/`);

    ws.onmessage = async (message) => {
        let data = await message.data.arrayBuffer();
        handler(parse(data));
    };

    ws.onclose = (event) => {
        handler(null);

        let waitTime = 1000;
        function retry() {
            fetch("/")
                // If fetching the page succeeds, the server is up.
                .then(() => location.reload())
                // Retry with exponential backoff.
                .catch(() => setTimeout(retry, (waitTime *= 1.1)));
        }
        setTimeout(retry, Math.random() * 1000);
    };
}

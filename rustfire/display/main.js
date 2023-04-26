import { connect } from "./socket";
import { h, redraw } from "./svg";

connect((state) => {
    // Show a message when disconnected.
    if (state === null) {
        redraw(
            "0 0 300 50",
            <text
                x="150"
                y="25"
                font-size="12px"
                fill="#ccc"
                text-anchor="middle"
                dominant-baseline="central"
            >
                Tappade kontakten till servern
            </text>
        );
        return;
    }

    let viewBox = state.view_box.join(" ");
    let dy = state.view_box[3] + state.view_box[1] * 2;
    let flip = `translate(0 ${dy}) scale(1, -1)`;

    let {
        offset: { x: rx, y: ry },
        rotation: r,
    } = state.robot;
    let robot = `translate(${rx} ${ry}) rotate(${(r * 180) / Math.PI})`;

    let target = null;
    if (state.target) {
        let { x: tx, y: ty } = state.target;
        target = `translate(${tx} ${ty})`;
    }

    redraw(
        viewBox,
        <g transform={flip} stroke-linecap="round" stroke-linejoin="round">
            <g stroke="#444" stroke-width={10}>
                {state.walls.map(({ p1, p2 }) => (
                    <line x1={p1.x} y1={p1.y} x2={p2.x} y2={p2.y} />
                ))}
            </g>
            <g fill="#8c8">
                {state.points.map(({ x, y }) => (
                    <circle cx={x} cy={y} r={5} />
                ))}
            </g>
            <g transform={robot} stroke="#ccc" stroke-width={10}>
                <circle x={0} y={0} r={120} fill="#fff3" />
                <path d="M0-80V80L-30 50M0 80L30 50" fill="none" />
            </g>
            {target && (
                <g transform={target} stroke="#cc0" stroke-width={10}>
                    <circle x={0} y={0} r={50} fill="#ff03" />
                </g>
            )}
        </g>,
        <text
            x={state.view_box[0]}
            y={state.view_box[1] + 50}
            font-size={50}
            fill="#ccc"
        >
            cost: {state.cost.toFixed(2)}
        </text>
    );
});

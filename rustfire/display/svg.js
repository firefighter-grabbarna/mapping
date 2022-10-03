let svg = document.getElementById("display");

/// Updates the children of the node to match the VDOM.
function updateChildren(parent, vdom) {
    // Special case with only one child, that happens to be a list.
    while (vdom.length === 1 && Array.isArray(vdom[0])) {
        vdom = vdom[0];
    }

    let oldNodes = Array.from(parent.childNodes);

    for (let i = 0; i < vdom.length; i++) {
        let oldNode = oldNodes[i];
        let newNode = updateNode(oldNode, vdom[i]);
        if (!oldNode) {
            parent.appendChild(newNode);
        } else if (newNode !== oldNode) {
            parent.replaceChild(newNode, oldNode);
        }
    }
    for (let i = vdom.length; i < oldNodes.length; i++) {
        parent.removeChild(oldNodes[i]);
    }
}

/// Updates the node to match the VDOM, returning the modified node.
function updateNode(node, vdom) {
    // Special values
    if (vdom === true || !vdom) vdom = "";
    if (Array.isArray(vdom)) vdom = h("g", null, ...vdom);

    if (typeof vdom === "object") {
        let attrs = vdom.attrs || {};
        // Create a new node if the old one can't be reused.
        if (!node || node.nodeName !== vdom.kind) {
            node = document.createElementNS(
                "http://www.w3.org/2000/svg",
                vdom.kind
            );
        }
        // Update the attributes and children.
        for (let { name } of node.attributes) {
            if (!Object.hasOwn(attrs, name)) {
                node.removeAttribute(name);
            }
        }
        for (let [name, value] of Object.entries(attrs)) {
            node.setAttribute(name, value);
        }
        updateChildren(node, vdom.children);
    } else {
        // Create a new text node if the old one can't be reused.
        if (!node || node.nodeName === "#text") {
            node = document.createTextNode(vdom);
        }
        // Update the node content if necessary.
        else if (node.textContent !== vdom) {
            node.textContent = vdom;
        }
    }
    return node;
}

/// Helper to create VDOM nodes with JSX syntax.
export function h(kind, attrs, ...children) {
    return { kind, attrs, children };
}

/// Updates the view box and replaces the contents of the canvas with the
/// specified nodes.
export function redraw(viewBox, ...content) {
    svg.setAttribute("viewBox", viewBox);
    updateChildren(svg, content);
}

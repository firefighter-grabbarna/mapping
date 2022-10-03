#include "nodemap.hpp"

#include <iostream>
#include <cstdlib>

#include "../common/math.hpp"

#include <vector>
#include <queue>

Node::Node(int node_x, int node_y, Point bound_1, Point bound_2, std::string name) {
    this->point = Point(node_x, node_y);

    this->bound_1 = bound_1;
    this->bound_2 = bound_2;
    this->name = name;
    this->visited = false;
    this->previous = nullptr;
}

std::vector<Node*> Node::getNeighbours() {
    return this->neighbours;
}

void Node::addNeighbours(Node *neighbour) {
    this->neighbours.push_back(neighbour);
}

void Node::setNeighbours(std::vector<Node*> neighbours) {
    this->neighbours = neighbours;
}

Point Node::getPoint() {
    return point;
}

std::string Node::getName() {
    return name;
}

bool Node::isVisited() {
    return visited;
}

void Node::setVisited(bool val) {
    visited = val;
}

void Node::setPrevious(Node* node) {
    previous = node;
}

Node* Node::getPrevious() {
    return previous;
}


bool Node::inBounds(int x, int y) {
    if (bound_1.x <= x && x <= bound_2.x &&
        bound_2.y <= y && y <= bound_1.y) return true;
    return false;
}

NodeMap::NodeMap() {
    //Middle node 
    Node* node_middle = new Node(950, 1180, Point(-1, -1), Point(-1, -1), "Middle");
    //Node 1
    Node* node_1 = new Node(935, 2165, Point(700, 2400), Point(2400, 1930),"Node 1");

    //Node 2
    Node* node_2 = new Node(2160, 1140, Point(1920, 1930), Point(2400, 900), "Node 2");

    //Node 3
    Node* node_3 = new Node(1405, 1140, Point(1170, 1380), Point(1920, 900), "Node 3");

    //Node 4
    Node* node_4 = new Node(235, 1255, Point(0, 1490), Point(710, 1020), "Node 4");

    //Node 5
    Node* node_5 = new Node(945, 670, Point(710, 900), Point(1180, 440), "Node 5");

    //Node 6
    Node* node_6 = new Node(945, 220, Point(710, 440), Point(1180, 0), "Node 6");

    //r1
    Node* r1 = new Node(1405, 1500, Point(1170, 1930), Point(1920, 1380), "Room 1");

    //r2
    Node* r2 = new Node(235, 1800, Point(0, 2400), Point(700, 1490), "Room 2");

    //r3
    Node* r3 = new Node(500, 220, Point(0, 1020), Point(700, 0), "Room 3");

    //R4
    Node* r4 = new Node(1400, 670, Point(1180, 900), Point(2400, 0), "Room 4");

    //Middle node neighbours
    node_middle->addNeighbours(node_1);
    node_middle->addNeighbours(node_3);
    node_middle->addNeighbours(node_4);
    node_middle->addNeighbours(node_5);

    //Node 1 neighbours
    node_1->addNeighbours(node_middle);

    //Node 2 neighbours
    node_2->addNeighbours(node_3);

    //Node 3 neighbours
    node_3->addNeighbours(node_middle);
    node_3->addNeighbours(node_2);
    node_3->addNeighbours(r1);

    //Node 4 neighbours
    node_4->addNeighbours(node_middle);
    node_4->addNeighbours(r2);

    //Node 5 neighbours
    node_5->addNeighbours(node_middle);
    node_5->addNeighbours(node_6);
    node_5->addNeighbours(r4);

    //Node 6 neighbours
    node_6->addNeighbours(r3);
    node_6->addNeighbours(node_5);

    //Room 1 neighbours
    r1->addNeighbours(node_3);

    //Room 2 neighbours
    r2->addNeighbours(node_4);

    //Room 3 neighbours
    r3->addNeighbours(node_6);

    //Room 4 neighbours
    r4->addNeighbours(node_5);

    nodes.push_back(node_middle);
    nodes.push_back(node_1);
    nodes.push_back(node_2);
    nodes.push_back(node_3);
    nodes.push_back(node_3);
    nodes.push_back(node_4);
    nodes.push_back(node_5);
    nodes.push_back(node_6);
    nodes.push_back(r1);
    nodes.push_back(r2);
    nodes.push_back(r3);
    nodes.push_back(r4);

    rooms.push_back(r1);
    rooms.push_back(r2);
    rooms.push_back(r3);
    rooms.push_back(r4);
}

NodeMap::~NodeMap() {

}

Node* NodeMap::getCurrentNode(int x, int y) {
    for (Node* node : nodes) {
        if (node->inBounds(x, y)) {
            return node;
        }
    }
    return nodes[0];
}

std::vector<Point> NodeMap::getPath(Node *startNode, Node *endNode) {

    std::queue<Node*> processingQueue;
    std::vector<Point> path;

    reset_visited();

    processingQueue.push(startNode);
    startNode->setVisited(true);

    Node* currentNode;
    while (!processingQueue.empty()) {
        currentNode = processingQueue.front();
        processingQueue.pop();
        if (currentNode == endNode) {
            recreatePath(startNode, endNode, path);
            return path;
        }
        else {
            for (Node* neighbour : currentNode->getNeighbours()) {
                if (!neighbour->isVisited()) {
                    neighbour->setVisited(true);
                    neighbour->setPrevious(currentNode);
                    processingQueue.push(neighbour);
                }
            }
        }
    }
    path.push_back(startNode->getPoint());
    return path;
}

void NodeMap::reset_visited() {
    for (Node *node : nodes) node->setVisited(false);
}

void NodeMap::recreatePath(Node* start, Node* end, std::vector<Point> &path) {
    std::vector<Point> backWardPath;
    Node* currentNode = end;

    while(currentNode != start) {
        backWardPath.push_back(currentNode->getPoint());
        currentNode = currentNode->getPrevious();
    }
    backWardPath.push_back(currentNode->getPoint());

    for (int i = backWardPath.size() -1; i > -1; --i) path.push_back(backWardPath[i]);
}

Node* NodeMap::getRoomNode(int i) {
    if (i > 4) return rooms[0];
    return rooms[i - 1];
}

std::vector<int> NodeMap::getRoomSearchNumber(int x, int y) {

    int currentRoomNumber = getCurrentRoomNumber(x, y);
    if (currentRoomNumber == 1) return {2, 4, 3};
    else if (currentRoomNumber == 2) return {1, 4, 3};
    else if (currentRoomNumber == 3) return {4, 1, 2};
    else return {3, 1, 2};
}

int NodeMap::getCurrentRoomNumber(int x, int y) {
    int i = 1;
    Node* currentNode = getCurrentNode(x, y);
    for (Node* node : rooms) {
        std::cout << node->getName() << " " << currentNode->getName() << std::endl;
        if (node->getName() == currentNode->getName()) return i;
        ++i;
    }
    return -1;
}

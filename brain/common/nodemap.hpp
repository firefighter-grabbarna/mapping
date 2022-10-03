#pragma once

#include "../common/math.hpp"

#include <vector>
#include <string>

class Node {
    public:
        Node(int node_x, int node_y, Point bound_1, Point bound_2, std::string name);

        std::vector<Node*> getNeighbours();

        void addNeighbours(Node *neighbour);

        void setNeighbours(std::vector<Node*> neighbours);

        Point getPoint();

        std::string getName();

        bool isVisited();

        void setVisited(bool val);

        void setPrevious(Node* node);

        Node* getPrevious();


        bool inBounds(int x, int y);
    private:
        int node_x, node_y;
        Point point, bound_1, bound_2;
        std::vector<Node*> neighbours;
        std::string name;
        Node* previous;
        bool visited;
};

class NodeMap {
    public:
        NodeMap();

        ~NodeMap();

        Node* getCurrentNode(int x, int y);

        std::vector<Point> getPath(Node *startNode, Node *endNode);

        void reset_visited();

        void recreatePath(Node* start, Node* end, std::vector<Point> &path);

        Node* getRoomNode(int i);

        std::vector<int> getRoomSearchNumber(int x, int y);

        int getCurrentRoomNumber(int x, int y);
    private:
        std::vector<Node*> nodes;
        std::vector<Node*> rooms;
};

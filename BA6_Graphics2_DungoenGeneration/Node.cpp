#include "Node.h"
#include "Edge.h"
#include <iostream>



//empty node
Node::Node()
{
    this->Id = 0;
    this->Container = Rect();
    this->Room = Rect();
	this->Difficulty = TileType::None;

    this->Left = nullptr;
    this->Right = nullptr;
    this->Parent = nullptr;
}

//branch node
Node::Node(int id, Node* parent, Rect container)
{
    this->Id = id;
    this->Container = container;
    this->Room = Rect();
    this->Difficulty = TileType::None;

    this->Left = nullptr;
    this->Right = nullptr;
    this->Parent = parent;
}

//-1 outside, 0 just inside-wall, 1 inside room
int Node::TileStateInsideRoom(int x, int y) {
    if(Room.x < x && x < Room.x + Room.width && Room.y < y && y < Room.y + Room.height) return 1;
    else if(Room.x <= x && x <= Room.x + Room.width && Room.y <= y && y <= Room.y + Room.height) return 0;
    else return -1;
}

void Node::GetLeaves(Node* thisNode, std::vector<Node*>& outLeaves) {
    if (thisNode->Left == nullptr && thisNode->Right == nullptr) {
        outLeaves.push_back(thisNode);
    }
    else {
        GetLeaves(thisNode->Left, outLeaves);
        GetLeaves(thisNode->Right, outLeaves);
    }
}

Edge* Node::GetEdgeTo(Node* node) {
    //find the edge which is connecting this node and the parameter node

    //this is wrong here Edges[i]->LinkedTo(node)->Id
    for (int i = 0; i < Edges.size(); i++) {
        if (Edges[i]->IsLinkedTo(node)) {
            //std::cout << "IT SHULD BE " << Id << " " << Edges[i]->NodeA->Id << " wa " << Edges[i]->NodeB->Id << " " << node->Id << "\n";;
            return Edges[i];
        }
    }

    return nullptr;
}





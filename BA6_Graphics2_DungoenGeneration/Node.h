#pragma once
#include "Rect.h"
#include <vector>
#include "DifficultyType.h"
#include "LockKeyPair.h"

class Edge;

class Node
{
public:
    int Id;
    TileType Difficulty;
    Rect Container;
    Rect Room;
    LockKeyPair* Key = nullptr;

    std::vector<Node*> ConnectedNodes; //in case of branches used to identify children, in case of leves used to identify sibling pair
    std::vector<Edge*> Edges; //doors 

    int DepthFromStart = 0;

    //during generation process
    Node* Left = nullptr;
    Node* Right = nullptr;
    Node* Parent = nullptr;

	Node(); //empty node
	Node(int id, Node* parent, Rect container); //branch node
    int TileStateInsideRoom(int x, int y);
    void GetLeaves(Node* thisNode, std::vector<Node*>& outLeaves);
    Edge* GetEdgeTo(Node* node);
};

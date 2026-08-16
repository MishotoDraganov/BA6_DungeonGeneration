#pragma once
#include "Node.h"
#include "LockKeyPair.h"

class Edge
{
public:
    Node* NodeA;
    Node* NodeB;

    LockKeyPair* Lock = nullptr;
    Node* LockedDoorNode = nullptr;

    Edge();
    Edge(Node* nodeA, Node* nodeB); 
    bool IsLinkedTo(Node* node);
    
    Node* LinkedTo(Node* node);
};

#include "Edge.h"

Edge::Edge()
{
    this->NodeA = new Node();
    this->NodeB = new Node(); 
}
Edge::Edge(Node* nodeA, Node* nodeB)
{
    this->NodeA = nodeA; 
    this->NodeB = nodeB;
}
Node* Edge::LinkedTo(Node* node)
{
    if (node == NodeA)
        return NodeB;
    
    if (node == NodeB)
        return NodeA;
    
    return nullptr;
}

bool Edge::IsLinkedTo(Node* node) {
    return (node == NodeA || node == NodeB);
}

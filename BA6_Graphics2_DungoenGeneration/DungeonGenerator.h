#pragma once
#include <vector>
#include <string>
#include "Rect.h"
#include "Node.h"
#include "Edge.h"



class DungeonGenerator
{
public:
    //creation process
    Node* rootNode;

    //final connections
    std::vector<Node*> LeafNodes;
    std::vector<Edge*> Edges;

    std::vector<Node*> CriticalPath, NonCrtiticalPath, EndNodes, KeyNodes;
    Node* StartNode;
    Node* BossNode;
    std::vector<Edge*> LockedEdges;


    DungeonGenerator(int w, int h, unsigned int seed);
    void Clear();
    void FillTiles(Node& node);
    void FillTiles(Edge& edge);
    void GenerateBSP();
    float EvaluateScore();


    void PrintASCII() const;
    bool SavePNG(const std::string& filename, int upscaleFactor) const;
    std::vector<unsigned char> GetImageBufferPNG(int upscaleFactor) const;


private:
    int width;
    int height;
    std::vector<std::vector<unsigned char>> tiles;

    int minSize = 10;

    void BinarySpacePartition(Node* parentNode, int depth, int minSize, int& currentID);


    void CalculateRoomSize();
    void AssignDifficulty();


    void ConnectLeafSiblings();
    void ConnectCorridors(Node* thisNode);
    void PickRandomLeaf(Node* thisNode, Node*& outLeaf);
    void PickClosestLeavesFromDifferentBranches(Node* thisNode, std::vector<Node*>& closestLeaves);
    void ConnectNodesAndMakeEdge(Node* nodeA, Node* nodeB);

    void RecurseDifficulty(Node* currentNode, int depth, Node*& furthestNodeFromStart, int& furthestDepth);
    void FindEndNodes(Node* current, Node* previous, std::vector<Node*>& endNodes);
    void FindPathBetween(Node* startNode, Node* endNode, std::vector<int>& visited, std::vector<Node*> path, std::vector<Node*>& returnPath);

    void DistributeLocksAndKeys();
    void PlaceKeys(std::vector<Edge*>* lockedEdges, std::vector<Node*> reachableNodes, std::vector<Node*> visitedNodes, Node* startNode);
    void FindAccessibleNodes(Node* currentNode, std::vector<Edge*> lockedEdges, std::vector<Node*>& reachedNodes, std::vector<Edge*> reachedLockedEdges);

    bool IsNeighboringTile(int checkX, int checkY, int tileX, int tileY);
    void TryFillEdgeTile(Edge& edge, int x, int y);
};
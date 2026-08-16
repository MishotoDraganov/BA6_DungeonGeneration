#include <iostream>
#include <algorithm>
#include <string>
#include <queue>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include "dungeonGenerator.h"
#include "Utils.h"


DungeonGenerator::DungeonGenerator(int w, int h, unsigned int seed) {
	width = w;
	height = h;

	tiles = std::vector<std::vector<unsigned char>>(
		width,
		std::vector<unsigned char>(height, 0));

	std::srand(seed);

	//accounting for -1 on size to make white borders when printed; this is not necessary for the start X and Y due to antitouch in the roomsizecalc function
	rootNode = new Node(0, nullptr, Rect(0, 0, width-1, height-1));
}



#pragma region Visualization
void DungeonGenerator::PrintASCII() const {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
		{
			if (tiles[x][y] == 0)
				std::cout << " ";
			else
				std::cout << (int)tiles[x][y];
		}
		std::cout << "\n";
	}	
}

std::vector<unsigned char> DungeonGenerator::GetImageBufferPNG(int upscaleFactor) const
{
	int upscaledWidth = width * upscaleFactor;
	int upscaledHeight = height * upscaleFactor;

	std::vector<unsigned char> flat(upscaledWidth * upscaledHeight * 4);

	for (int y = 0; y < upscaledHeight; y++)
	{
		for (int x = 0; x < upscaledWidth; x++)
		{
			int tileX = x / upscaleFactor;
			int tileY = y / upscaleFactor;

			Color* color = Utils::IntDifficultyColorMap[tiles[tileX][tileY]];

			int index = (y * upscaledWidth + x) * 4;

			flat[index + 0] = color->r;
			flat[index + 1] = color->g;
			flat[index + 2] = color->b;
			flat[index + 3] = color->a;
		}
	}

	return flat;
}

bool DungeonGenerator::SavePNG(const std::string& filename, int upscaleFactor) const
{
	int upscaledWidth = width * upscaleFactor;
	int upscaledHeight = height * upscaleFactor;



	int result = stbi_write_png(
		filename.c_str(),
		upscaledWidth,
		upscaledHeight,
		4,                  //rgba
		GetImageBufferPNG(upscaleFactor).data(),
		upscaledWidth * 4  //bytes per row
	);
	// get all buffers
}

#pragma endregion


void DungeonGenerator::Clear() {
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			tiles[x][y] = 0;
}

void DungeonGenerator::GenerateBSP() {
	Utils::InitUtils();
	BinarySpacePartition(rootNode, 4, minSize, rootNode->Id);
	CalculateRoomSize();


	for (int i = 0; i < LeafNodes.size(); i++)
		ConnectCorridors(LeafNodes[i]);	

	AssignDifficulty();

	DistributeLocksAndKeys();


	//fill tile data
	for (int i = 0; i < LeafNodes.size(); i++) 
		FillTiles(*LeafNodes[i]);


	for (int i = 0; i < Edges.size(); i++) 
		FillTiles(*Edges[i]);	
}


void DungeonGenerator::BinarySpacePartition(Node* thisNode, int depth, int minSize, int& currentID) {

	if (thisNode->Parent != nullptr) {

		if (thisNode->Parent->Left == nullptr) 
			thisNode->Parent->Left = thisNode;		
		else if (thisNode->Parent->Right == nullptr) 
			thisNode->Parent->Right = thisNode;		
	}


	if (depth <= 0 || thisNode->Container.width < minSize * 2 || thisNode->Container.height < minSize * 2) {
		LeafNodes.push_back(thisNode);
		return;
	}




	bool splitVertical = (std::rand() % 2 == 0);


	if (thisNode->Container.AspectRatio() > 4.0f)
		splitVertical = true;
	else if (thisNode->Container.AspectRatio() < 0.25f)
		splitVertical = false;



	float cutOffPercentage = Utils::RandomRange(0.4f, 0.6f);
	int tried = 0;
	while (cutOffPercentage * thisNode->Container.width < minSize*2 || cutOffPercentage * thisNode->Container.height < minSize*2) {
		cutOffPercentage = Utils::RandomRange(0.4f, 0.6f);

		tried++;
		if (tried > 10) break;
	}




	if (splitVertical) {
		int split = thisNode->Container.x + (thisNode->Container.width * cutOffPercentage);

		Rect left = { thisNode->Container.x, thisNode->Container.y, split - thisNode->Container.x, thisNode->Container.height };
		Rect right = { split, thisNode->Container.y, thisNode->Container.x + thisNode->Container.width - split, thisNode->Container.height };

		//if(left.AspectRatio() > maxSizeRatio || right.AspectRatio() > maxSizeRatio)
		Node* leftNode = new Node(currentID+1, thisNode, left);
		Node* rightNode = new Node(currentID+2, thisNode, right);

		currentID++;
		BinarySpacePartition(leftNode, depth - 1, minSize, currentID);
		currentID++;
		BinarySpacePartition(rightNode, depth - 1, minSize, currentID);
	}
	else {

		int split = thisNode->Container.y + (thisNode->Container.height * cutOffPercentage);

		Rect top = { thisNode->Container.x, thisNode->Container.y, thisNode->Container.width, split - thisNode->Container.y };
		Rect bottom = { thisNode->Container.x, split, thisNode->Container.width, thisNode->Container.y + thisNode->Container.height - split };

		Node* topNode = new Node(currentID+1, thisNode, top);
		Node* bottomNode = new Node(currentID+2, thisNode, bottom);

		currentID++;
		BinarySpacePartition(topNode, depth - 1, minSize, currentID);	
		currentID++;
		BinarySpacePartition(bottomNode, depth - 1, minSize, currentID);
	}

}

void DungeonGenerator::ConnectNodesAndMakeEdge(Node* nodeA, Node* nodeB) {
	nodeA->ConnectedNodes.push_back(nodeB);	
	nodeB->ConnectedNodes.push_back(nodeA);	


	Edges.push_back(new Edge(nodeA, nodeB));
	nodeA->Edges.push_back(Edges[Edges.size() - 1]);
	nodeB->Edges.push_back(Edges[Edges.size() - 1]);

	//std::cout<<("Connected Node " + std::to_string(nodeA->Id) + " and Node " + std::to_string(nodeB->Id) + "\n");	
}

void DungeonGenerator::AssignDifficulty() {

	//assign starting room at center
	float closestDistToCenter = std::numeric_limits<float>::max();
	Node* startNode = LeafNodes[0];
	for (int i = 0; i < LeafNodes.size(); i++) {
		float distToCenter = Utils::Dist(LeafNodes[i]->Container.GetCenterPos(), { static_cast<float>(width) / 2, static_cast<float>(height) / 2 });
		if (distToCenter < closestDistToCenter) {
			closestDistToCenter = distToCenter;
			startNode = LeafNodes[i];
		}
	}
	startNode->Difficulty = TileType::Starting;


	//find boss room; assign regular rooms;
	Node* bossNode = nullptr;
	int furthestDepth = 2;
	RecurseDifficulty(startNode, furthestDepth, bossNode, furthestDepth);


	//assign treasure as rooms not on the critical path from starting to boss
	std::vector<int> visitedIds;
	std::vector<Node*> path;
	FindPathBetween(startNode, bossNode, visitedIds, path, CriticalPath);

	//end nodes not part of the critical path may become a treasure room
	std::copy_if(LeafNodes.begin(),LeafNodes.end(),std::back_inserter(NonCrtiticalPath),[&](Node* node)
		{
			return std::find(CriticalPath.begin(), CriticalPath.end(), node) == CriticalPath.end();
		}
	);

	FindEndNodes(startNode, startNode, EndNodes);
	for(Node* node : EndNodes)
		node->Difficulty = TileType::Treasure;


	//assign boss node
	bossNode->Difficulty = TileType::Boss;

	BossNode = bossNode;
	StartNode = startNode;
}

void DungeonGenerator::DistributeLocksAndKeys() {
	// place locks on doors, on rooms on the critical path
	//place keys on rooms, on the NON-critical path

	//colors
	LockKeyPair* keyLockPair1 = new LockKeyPair(TileType::LockKey1);
	LockKeyPair* keyLockPair2 = new LockKeyPair(TileType::LockKey2);


	//locks
	int lockRoom1 = Utils::RandomRange(0, CriticalPath.size() - 2);
	int lockRoom2 = Utils::RandomRange(0, CriticalPath.size() - 2, lockRoom1);


	//switch them if 1 is bigger than 2
	if (lockRoom1 > lockRoom2) {
		int temp = lockRoom1;
		lockRoom1 = lockRoom2;
		lockRoom2 = temp;
	}
	//pick random edge along the critical path, assign lock color and set lockeddoornode so that whne drawn only the enddoor of the edge is displayed as locked
	std::vector<Edge*> lockedEdges;
	lockedEdges.push_back(CriticalPath[lockRoom1]->GetEdgeTo(CriticalPath[lockRoom1 + 1]));
	lockedEdges.push_back(CriticalPath[lockRoom2]->GetEdgeTo(CriticalPath[lockRoom2 + 1]));
	lockedEdges[0]->Lock = keyLockPair1;
	lockedEdges[1]->Lock = keyLockPair2;
	lockedEdges[0]->LockedDoorNode = CriticalPath[lockRoom1 + 1];
	lockedEdges[1]->LockedDoorNode = CriticalPath[lockRoom2 + 1];

	std::vector<Node*> keyNodes;
	//splits the nodes into sections based on the doors; then places a key for the door preventing it's passage; number of doors is number of cycles, as we cant put a key after the final door
	for (int i = 0; i < lockedEdges.size(); i++) {
		std::vector<Node*> reachedNodes;
		std::vector<Edge*> reachedLockedEdges;
		//start search from the node -1 of the door
		FindAccessibleNodes(lockedEdges[i]->LinkedTo(lockedEdges[i]->LockedDoorNode), lockedEdges, reachedNodes, reachedLockedEdges);

		std::vector<Node*> sharedNodes;
		for (Node* node : reachedNodes)
			if (std::find(EndNodes.begin(), EndNodes.end(), node) != EndNodes.end())
				sharedNodes.push_back(node);

		if (sharedNodes.size() > 0) {
			int tempI = Utils::RandomRange(0, sharedNodes.size() - 1);
			sharedNodes[tempI]->Key = lockedEdges[i]->Lock;
			keyNodes.push_back(sharedNodes[tempI]);

		}
		else {
			int tempI = Utils::RandomRange(0, reachedNodes.size() - 1);
			reachedNodes[tempI]->Key = lockedEdges[i]->Lock;
			keyNodes.push_back(reachedNodes[tempI]);
		}
	}

	LockedEdges = lockedEdges;
	KeyNodes = keyNodes;
}

void DungeonGenerator::FindEndNodes(Node* current,Node* previous, std::vector<Node*>& endNodes)
{
	bool hasUnvisitedNeighbor = false;

	for (Node* neighbor : current->ConnectedNodes)
	{
		if (neighbor == previous) continue;

		hasUnvisitedNeighbor = true;
		FindEndNodes(neighbor, current, endNodes);
	}

	if (!hasUnvisitedNeighbor) endNodes.push_back(current);
}
void DungeonGenerator::FindAccessibleNodes(Node* currentNode, std::vector<Edge*> lockedEdges, std::vector<Node*>& reachedNodes, std::vector<Edge*> reachedLockedEdges) {
	reachedNodes.push_back(currentNode);


	for (int i = 0; i < currentNode->ConnectedNodes.size(); i++) {

		//if linked node already visited, don't visit it again
		bool linkedNodeAlreadyVisited = false;
		Node* linkedNode = currentNode->ConnectedNodes[i];
		for (int j = 0; j < reachedNodes.size(); j++) {
			if (linkedNode == reachedNodes[j]) {
				linkedNodeAlreadyVisited = true;
				break;
			}
		}
		if (linkedNodeAlreadyVisited) { continue; }


		//skip locked edges
		bool isEdgeLocked = false;
		for (int j = 0; j < lockedEdges.size(); j++) {
			if (linkedNode == lockedEdges[j]->LinkedTo(currentNode)) {
				isEdgeLocked = true;
				reachedLockedEdges.push_back(currentNode->Edges[i]);
				break;
			}
		}
		if (isEdgeLocked) {continue;}


		//recurse; accessed nodes serves the function of visited nodes
		if (!isEdgeLocked && !linkedNodeAlreadyVisited) 
			FindAccessibleNodes(linkedNode, lockedEdges, reachedNodes, reachedLockedEdges);

	}
}
void DungeonGenerator::FindPathBetween(Node* startNode, Node* endNode, std::vector<int>& visited, std::vector<Node*> path, std::vector<Node*>& returnPath) {
	path.push_back(startNode);
	visited.push_back(startNode->Id);



	//if has not reached the end node, continue searching
	if (startNode->Id == endNode->Id) {
		returnPath = path;
		return;
	}
	else 
		for (int i = 0; i < startNode->ConnectedNodes.size(); i++) 
			if(!Utils::VectorContains(visited, startNode->ConnectedNodes[i]->Id)) 
				FindPathBetween(startNode->ConnectedNodes[i], endNode, visited, path, returnPath);

}





float DungeonGenerator::EvaluateScore() {


	//critical playthrough length
	std::vector<int> visited;
	std::vector<Node*> path;
	std::vector<Node*> returnPath;

	std::vector<Node*> sequence = {
		StartNode, 
		KeyNodes[0],
		LockedEdges[0]->LockedDoorNode,
		KeyNodes[1],
		LockedEdges[1]->LockedDoorNode,
		BossNode
	};


	int nodesPassed = 0;
	for (int i = 0; i < sequence.size() - 1; i++) {
		FindPathBetween(sequence[i], sequence[i + 1], visited, path, returnPath);
		path.clear();
		visited.clear();
		nodesPassed += returnPath.size();
	}
	float maxPathLengthScore = static_cast<float>(nodesPassed) / static_cast<float>(LeafNodes.size() + 10);
	if (maxPathLengthScore > 1.0f) maxPathLengthScore == 1.0f;


	//by difficulty
	float diffScore = -1;
	float treasureScore = 0;
	for (Node* node : LeafNodes) {
		int difficulty = static_cast<float>(node->Difficulty);
		if (difficulty < 5) diffScore += difficulty;
		if (difficulty == 5) treasureScore += 1;
	}
	diffScore = diffScore / 28;
	if (diffScore > 1) diffScore = 1;

	treasureScore = treasureScore / 2;
	if (treasureScore > 1) treasureScore = 1;

	float combinedScore = (maxPathLengthScore + diffScore + treasureScore) / 3;
	if (combinedScore > 1) combinedScore = 1.0f;
	std::cout << "combined score " <<combinedScore << " " << maxPathLengthScore << " " << diffScore << " " << treasureScore << "\n";

	return combinedScore;
}


void DungeonGenerator::RecurseDifficulty(Node* currentNode, int depth, Node*& furthestNodeFromStart, int& furthestDepth) {
	if (depth > furthestDepth) {
		furthestDepth = depth;
		furthestNodeFromStart = currentNode;
	}



	for (int i = 0; i < currentNode->ConnectedNodes.size(); i++) {
		if(currentNode->ConnectedNodes[i]->Difficulty == TileType::None) {

			//assign difficulty based on depth, if depth above 4 pick random difficulty
			currentNode->ConnectedNodes[i]->Difficulty = static_cast<TileType>(depth);
			if (depth > 4) 
				currentNode->ConnectedNodes[i]->Difficulty = static_cast<TileType>(Utils::RandomRange(2, 4));


			currentNode->ConnectedNodes[i]->DepthFromStart = depth + 1;
			RecurseDifficulty(currentNode->ConnectedNodes[i], depth+1, furthestNodeFromStart, furthestDepth);
		}
	}
}


void DungeonGenerator::ConnectLeafSiblings() {
	for (int i = 0; i < LeafNodes.size(); i++) {
		if (LeafNodes[i] == LeafNodes[i]->Parent->Left) { //only once per pair
			ConnectNodesAndMakeEdge(LeafNodes[i], LeafNodes[i]->Parent->Right);
		}	
	}
}


//accepts the last leaf node as a starting parameter
void DungeonGenerator::ConnectCorridors(Node* thisNode) {
	if (thisNode->Left != nullptr && thisNode->Right != nullptr) {
		//only 2 leaf nodes who are connected
		std::vector<Node*> closestLeaves;
		PickClosestLeavesFromDifferentBranches(thisNode, closestLeaves);
		ConnectNodesAndMakeEdge(closestLeaves[0], closestLeaves[1]);
	}



	//go up the tree and connect the next pair of nodes
	if(thisNode->Parent!=nullptr)
		ConnectCorridors(thisNode->Parent);
}

void DungeonGenerator::PickRandomLeaf(Node* thisNode, Node*& outLeaf) {
	if (thisNode->Left == nullptr && thisNode->Right == nullptr) {
		outLeaf = thisNode;
		return;
	}


	if (std::rand() % 2 == 0)
		PickRandomLeaf(thisNode->Left, outLeaf);
	else
		PickRandomLeaf(thisNode->Right, outLeaf);

}

void DungeonGenerator::PickClosestLeavesFromDifferentBranches(Node* thisNode, std::vector<Node*>& closestLeaves) {
	//need to go through all leaves, test their distance, dont have closest leaf, instead closest leaf pair from the 2 branches
	std::vector<Node*> leftLeaves;
	std::vector<Node*> rightLeaves;

	thisNode->Left->GetLeaves(thisNode->Left, leftLeaves);
	thisNode->Right->GetLeaves(thisNode->Right, rightLeaves);

	float minDist = std::numeric_limits<float>::max();
	//Node* closestLeaf = nullptr;

	Node* closestLeftLeaf = nullptr;
	Node* closestRightLeaf = nullptr;
	for (Node* leftLeaf : leftLeaves) {
		for (Node* rightLeaf : rightLeaves) {
			float dist = Utils::Dist(leftLeaf->Container.GetCenterPos(), rightLeaf->Container.GetCenterPos());
			if (dist < minDist) {
				minDist = dist;
				closestLeftLeaf = leftLeaf;
				closestRightLeaf = rightLeaf;
			}
		}
	}

	closestLeaves.push_back(closestLeftLeaf);
	closestLeaves.push_back(closestRightLeaf);
}





void DungeonGenerator::CalculateRoomSize() {
	for (int i = 0; i < LeafNodes.size(); i++) {
		//generate room on leaf nodes
		int antiTouch = 1;

		float maxSizePercentage = 0.8f;
		float minSizePercentage = 0.4f;

		float roomWidth = std::floor(Utils::RandomRange(minSizePercentage * static_cast<float>(LeafNodes[i]->Container.width), maxSizePercentage *  static_cast<float>(LeafNodes[i]->Container.width)));
		float roomHeight = std::floor(Utils::RandomRange(minSizePercentage * static_cast<float>(LeafNodes[i]->Container.height), maxSizePercentage *  static_cast<float>(LeafNodes[i]->Container.height)));


		int ty = static_cast<int>(LeafNodes[i]->Container.GetCenterPos().y - (roomHeight) / 2);
		int tx = static_cast<int>(LeafNodes[i]->Container.GetCenterPos().x - (roomWidth) / 2);
		Rect roomSize = { 
			tx - antiTouch,
			ty + antiTouch,
			static_cast<int>(roomWidth) - antiTouch,
			static_cast<int>(roomHeight) - antiTouch
		};
		LeafNodes[i]->Room = roomSize;
	}
}



bool DungeonGenerator::IsNeighboringTile(int checkX, int checkY, int tileX, int tileY) {
	for (int y = tileY - 1; y < tileY + 2; y++)
		if (checkY == y)
			for (int x = tileX - 1; x < tileX + 2; x++)
				if (checkX == x) return true;


	return false;
}
//if has key or lock do the speical filltiling with placing key on cneter and lock on door, both with the color tying them together; which reminds me of needing a way to link the 2
void DungeonGenerator::FillTiles(Node& node) {

	for (int x = node.Container.x; x < node.Container.x + node.Container.width; x++) {
		for (int y = node.Container.y; y < node.Container.y + node.Container.height; y++) {
			if (node.TileStateInsideRoom(x, y) == 1)
				tiles[x][y] = (unsigned char)node.Difficulty;
			else if (node.TileStateInsideRoom(x, y) == 0)
				tiles[x][y] = (unsigned char)TileType::Wall;
			else
				tiles[x][y] = 0;

			if (node.Key != nullptr && IsNeighboringTile(x, y, node.Room.GetCenterPos().x, node.Room.GetCenterPos().y))
				tiles[x][y] = (unsigned char)node.Key->sharedTileType;
		}
	}
}



//this is deeply annoying, instead of doing this i will shoot vectors lerp through them then place tiles on current point of vector floored
void DungeonGenerator::FillTiles(Edge& edge) {


	Vector2 centerA = edge.NodeA->Room.GetCenterPos();
	Vector2 centerB = edge.NodeB->Room.GetCenterPos();
	Vector2 closestDoorOnA =centerA, closestDoorOnB = centerB;



	Vector2 direction = Vector2(closestDoorOnB.x - closestDoorOnA.x, closestDoorOnB.y - closestDoorOnA.y);
	float distance = Utils::Dist(closestDoorOnA, closestDoorOnB);

	for (float t = 0.0f; t <= 1.0f; t += 0.01f / distance)
	{
		Vector2 current = Utils::Lerp(closestDoorOnA, closestDoorOnB, t);

		int x = static_cast<int>(std::floor(current.x));
		int y = static_cast<int>(std::floor(current.y));

		TryFillEdgeTile(edge, x, y);
	}
}

void DungeonGenerator::TryFillEdgeTile(Edge& edge, int x, int y) {
	if (x >= tiles.size() || y >= tiles[x].size()) return;

	if(tiles[x][y] == (unsigned char)TileType::None) tiles[x][y] = (unsigned char)TileType::Corridor;

	if (tiles[x][y] == (unsigned char)TileType::Wall) {
		bool onLockedDoorTile = edge.Lock != nullptr && edge.LockedDoorNode!=nullptr && edge.LockedDoorNode->TileStateInsideRoom(x, y) == 0;
		if (onLockedDoorTile && !edge.Lock->lockAlreadyPrinted) {
			edge.Lock->lockAlreadyPrinted = true;

			for(int i=x-1; i<x+2; i++)
				for(int j=y-1;  j<y+2; j++)
					tiles[i][j] = (unsigned char)edge.Lock->sharedTileType;
		}
		else
			tiles[x][y] = (unsigned char)TileType::Door;
	}
}


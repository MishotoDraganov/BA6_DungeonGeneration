#include "Utils.h"
#include <cstdlib>
#include <random>
#include "Node.h"




std::map<TileType, Color*> Utils::DifficultyColorMap;
std::map<int, Color*> Utils::IntDifficultyColorMap;




//inclusive on both ends
int Utils::RandomRange(int const min, int const max) {
	if (max <= min) return min;

	return min + std::rand() % (max - min + 1);
}

int Utils::RandomRange(int const min, int const max, int const excludingNumber) {
	if (max <= min) return min;

	int num = min + std::rand() % (max - min + 1);
	while (num == excludingNumber) num = min + std::rand() % (max - min + 1);

	return num;
}

float Utils::RandomRange(float const min, float const max) {
	if (max <= min) return min;

	return min + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

float Utils::RandomDistribution(float const min, float const max) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> dist(min, max);

	float value = dist(gen);
	return value;
}
float Utils::Dist(Vector2 A, Vector2 B) {
	float dx = A.x - B.x;
	float dy = A.y - B.y;
	return sqrt(dx * dx + dy * dy);
}

void Utils::InitUtils() {
	PopulateDifficultyColorMap();
}

void Utils::PopulateDifficultyColorMap() {
	DifficultyColorMap.insert({ TileType::None, new Color{ 0, 0, 0, 255 } });

	DifficultyColorMap.insert({ TileType::Starting, new Color{ 110, 180, 255, 255 } });
	DifficultyColorMap.insert({ TileType::Easy, new Color{ 140, 220, 190, 255 } });
	DifficultyColorMap.insert({ TileType::Medium, new Color{ 220, 120, 190, 255 } });
	DifficultyColorMap.insert({ TileType::Hard, new Color{ 170, 110, 225, 255 } });
	DifficultyColorMap.insert({ TileType::Treasure, new Color{ 255, 205, 70, 255 } });
	DifficultyColorMap.insert({ TileType::Boss, new Color{ 245, 75, 90, 255 } });



	DifficultyColorMap.insert({ TileType::Corridor, new Color{ 100, 190, 180, 255 } });
	DifficultyColorMap.insert({ TileType::Wall, new Color{ 45, 75, 80, 255 } });
	DifficultyColorMap.insert({ TileType::Door, new Color{ 80, 225, 210, 255 } });



	DifficultyColorMap.insert({ TileType::LockKey1, new Color{ 180, 40, 40, 255 } });
	DifficultyColorMap.insert({ TileType::LockKey2, new Color{ 40, 180, 40, 255 } });
	DifficultyColorMap.insert({ TileType::LockKey3, new Color{ 40, 80, 200, 255 } });
	DifficultyColorMap.insert({ TileType::LockKey4, new Color{ 120, 40, 180, 255 } });

	for (const auto& pair : DifficultyColorMap) {
		IntDifficultyColorMap.insert({ static_cast<int>(pair.first), pair.second });
	}
}

bool Utils::VectorContains(std::vector<int> vector, int value) {
	for (int i = 0; i < vector.size(); i++)
		if (vector[i] == value)
			return true;


	return false;
}

Vector2 Utils::Lerp(const Vector2& a, const Vector2& b, float t)
{
	Vector2 temp = Vector2(b.x - a.x, b.y - a.y) * t;
	return Vector2(a.x + temp.x, a.y + temp.y);
}

bool Utils::VectorContainsNode(const std::vector<Node*>& nodes, const Node& checkedNode){
	for (Node* node : nodes)
		if (node->Id == checkedNode.Id) 
			return true;
	

	return false;
}


 /*
 bool Utils::DrawPNG(std::vector<unsigned char>& image,int imageWidth,int imageHeight, const std::string& filename,int posX,int posY)
 {
	 int width, height, channels;

	 unsigned char* png = stbi_load(filename.c_str(),&width,&height,&channels,4);

	 if (!png)
		 return false;

	 for (int y = 0; y < height; y++)
		 for (int x = 0; x < width; x++)
		 {
			 int destX = posX + x;
			 int destY = posY + y;

			 if (destX < 0 || destX >= imageWidth || destY < 0 || destY >= imageHeight)
				 continue;
			 

			 int sourceIndex = (y * width + x) * 4;
			 int destIndex = (destY * imageWidth + destX) * 4;

			 image[destIndex + 0] = png[sourceIndex + 0];
			 image[destIndex + 1] = png[sourceIndex + 1];
			 image[destIndex + 2] = png[sourceIndex + 2];
			 image[destIndex + 3] = png[sourceIndex + 3];
		 }
	 

	 stbi_image_free(png);

	 return true;
 }
 */

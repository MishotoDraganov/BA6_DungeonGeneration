#pragma once
#include "Vector2.h"
#include <map>
#include "DifficultyType.h"
#include "Color.h"
#include <vector>
#include <string>

class Node;


class Utils
{
public:
    static std::map<TileType, Color*> DifficultyColorMap;
    static std::map<int, Color*> IntDifficultyColorMap;


	static void InitUtils();

    static int RandomRange(const int min, const int max);
    static int RandomRange(int const min, int const max, int const excludingNumber);

    static float RandomRange(const float min, const float max);
    static float RandomDistribution(const float min, const float max);

    static float Dist(Vector2 A, Vector2 B);
    static bool VectorContains(std::vector<int> vector, int value);
    static Vector2 Lerp(const Vector2& a, const Vector2& b, float t);
    //static  bool DrawPNG(std::vector<unsigned char>& image, int imageWidth, int imageHeight, const std::string& filename, int posX, int posY);
    static bool VectorContainsNode(const std::vector<Node*>& nodes, const Node& checkedNode);

private:
	static void PopulateDifficultyColorMap();
};

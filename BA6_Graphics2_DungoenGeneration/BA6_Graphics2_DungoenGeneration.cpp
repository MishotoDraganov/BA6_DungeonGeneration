#include <iostream>
#include "DungeonGenerator.h"
#include <string>
int main()
{
	for (int seed = 0; seed < 16; seed++) {
		DungeonGenerator dungeon = DungeonGenerator(100, 100, seed);
		dungeon.Clear();
		dungeon.GenerateBSP();
		//dungeon.PrintASCII();


		std::string combined = " seed(" +std::to_string(seed)+") score " + std::to_string(dungeon.EvaluateScore());


		//method that draws an image inside a bigger image -> when drawing if the pixel will be drawn past the size specified, carry it over to the bottom row, asit takes a vector<> input
		//draw 16 times in this for loop, moving to the bottom row if it reaches the end	

		if (dungeon.SavePNG(combined + ".png", 10)) 
			std::cout << "PNG saved successfully\n";
		else 
			std::cout << "FAILED to GENERATE\n";

	}
	return 0;
}


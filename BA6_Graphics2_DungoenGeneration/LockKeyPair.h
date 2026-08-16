#pragma once
#include "DifficultyType.h"

struct LockKeyPair {
	TileType sharedTileType = TileType::None;
	bool lockAlreadyPrinted = false;
};
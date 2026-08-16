#pragma once
#include "Color.h"

struct LockKey {
	int id;
	Color color;

	LockKey(int id, Color color);
};
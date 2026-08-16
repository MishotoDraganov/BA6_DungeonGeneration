#pragma once

struct Vector2
{
	float x = 0;
	float y = 0;

	Vector2(float x, float y) : x(x), y(y) {}
	Vector2() : x(0), y(0) {}   // add this

	Vector2 operator*(float scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}
};

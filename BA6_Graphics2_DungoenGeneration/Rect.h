#pragma once
#include <vector>
#include "Vector2.h"


struct Rect
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    
    Vector2 center = Vector2();
    std::vector<Vector2> doors;

    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}


    float AspectRatio();
    Vector2 GetCenterPos();
    std::vector<Vector2>& GetDoorsPos();
    Vector2 GetClosestDoorPos(Vector2 point);
};

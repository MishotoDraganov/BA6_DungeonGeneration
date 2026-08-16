#include "Rect.h"
//#include <algorithm>
#include "Utils.h"

float Rect::AspectRatio()
{
    float w = static_cast<float>(width);
    float h = static_cast<float>(height);

    //return std::max(w / h, h / w);
    return w / h;
}

Vector2 Rect::GetCenterPos()
{
    //if not value set`
    if(center.x == 0 && center.y == 0)
        center = Vector2(width / 2 + x,  height / 2 + y);  

    return center;
}

std::vector<Vector2>& Rect::GetDoorsPos() {

    if (doors.size() == 0) {
        GetCenterPos();

        doors.push_back(Vector2(center.x - width/2, center.y)); //left
        doors.push_back(Vector2(center.x + width/2, center.y)); //right
        doors.push_back(Vector2(center.x, center.y - height/2)); //up?
        doors.push_back(Vector2(center.x, center.y + height/2)); //down?
    }

    return doors;
}
Vector2 Rect::GetClosestDoorPos(Vector2 point) {
    GetDoorsPos();

    float closestDoorDist = std::numeric_limits<float>::max();
    Vector2 closestDoorPos;

    for (Vector2 door : doors)
        if (Utils::Dist(point, door) < closestDoorDist) {
            closestDoorDist = Utils::Dist(point, door);
            closestDoorPos = door;
        }
    

    return closestDoorPos;
}
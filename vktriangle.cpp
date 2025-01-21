#include "vktriangle.h"

VKTriangle::VKTriangle(){}

VKTriangle::VKTriangle(VKVertex vertex1, VKVertex vertex2, VKVertex vertex3)
{
    mVertices[0] = vertex1;
    mVertices[1] = vertex2;
    mVertices[2] = vertex3;
}

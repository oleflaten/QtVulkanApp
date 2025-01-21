#ifndef VKTRIANGLE_H
#define VKTRIANGLE_H

#include "vkvertex.h"

class VKTriangle
{
public:
    VKTriangle();
    VKTriangle(VKVertex vertex1, VKVertex vertex2, VKVertex vertex3);

    VKVertex* getVertices() { return mVertices; }

    VKVertex mVertices[3];
};

#endif // VKTRIANGLE_H

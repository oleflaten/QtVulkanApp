#ifndef VKTRIANGLEMESH_H
#define VKTRIANGLEMESH_H

#include <vector>
#include "vktriangle.h"

class VKTriangleMesh
{
public:
    VKTriangleMesh();

    std::vector<VKTriangle> mTriangles;

    std::vector<VKTriangle> getTriangles() { return mTriangles; }

    void addTriangle(const VKTriangle& triangle);

    int getSize();
};

#endif // VKTRIANGLEMESH_H

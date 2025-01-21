#include "vktrianglemesh.h"

VKTriangleMesh::VKTriangleMesh() {}

void VKTriangleMesh::addTriangle(const VKTriangle& triangle)
{
    mTriangles.push_back(triangle);
}

int VKTriangleMesh::getSize()
{
    return mTriangles.size() * sizeof(VKTriangle);
}

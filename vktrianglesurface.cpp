#include "vktrianglesurface.h"

#include <fstream>
VkTriangleSurface::VkTriangleSurface() : VisualObject()
{
    VKVertex v1{0.0f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    VKVertex v2{1.0f,   0.0f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    VKVertex v3{0.0f,   1.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    VKVertex v4{1.0f,   1.0f,  0.0f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    mVertices.push_back(v3);
    mVertices.push_back(v2);
    mVertices.push_back(v4);
}

VkTriangleSurface::VkTriangleSurface(const std::string &filename)
{
    std::ifstream inn(filename);
    if (!inn.is_open())
        return;
    // read input from math part of compulsory
    int n;
    VKVertex v;
    inn >> n;
    for (auto i=0; i<n; i++)
    {
        inn >> v;
        mVertices.push_back(v);
        //qDebug() << v.x << v.y << v.z;
    }
    inn.close();
}

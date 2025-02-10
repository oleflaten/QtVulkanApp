#include "VkTriangle.h"
#include "Vertex.h"
#include "VisualObject.h"

VkTriangle::VkTriangle() : VisualObject()
{
    //Vertex v1{-0.5f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    //Vertex v2{-0.5f,   -0.5f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    //Vertex v3{0.0f,   0.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

    mVertices.push_back(Vertex{-0.5f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    mVertices.push_back(Vertex{-0.5f,   -0.5f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f});
    mVertices.push_back(Vertex{0.0f,   0.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f});

    mMatrix.translate(-0.25f, 0, 0); // fra startNextFrame
}

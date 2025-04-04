#include "Triangle.h"
#include "Vertex.h"
#include "VisualObject.h"

Triangle::Triangle() : VisualObject()
{
    mVertices.push_back(Vertex{-0.5f,   0.0f,  0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f});
    mVertices.push_back(Vertex{-0.5f,  -0.5f,  0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f});
    mVertices.push_back(Vertex{ 0.0f,   0.0f,  0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f});

	//Temporary positioning
    mMatrix.translate(-0.25f, 0, 0);

    drawType = 2; // 0 = fill color, 1 = line color, 2 = fill phong
}

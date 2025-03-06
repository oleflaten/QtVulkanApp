#include "Triangle.h"
#include "Vertex.h"
#include "VisualObject.h"

Triangle::Triangle() : VisualObject()
{
    mVertices.push_back(Vertex{-0.5f,   0.0f,  0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    mVertices.push_back(Vertex{-0.5f,   -0.5f,  0.0f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f});
    mVertices.push_back(Vertex{0.0f,   0.0f,  0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f});

	//Temporary positioning
    mMatrix.setToIdentity();
    mMatrix.translate(-0.25f, 0, 0);
}

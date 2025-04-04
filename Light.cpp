#include "Light.h"

Light::Light()
{
    //Other way to push vertices directly into a std::vector
    mVertices.insert( mVertices.end(),
        {
            Vertex{-0.25f, -0.25f,  0.25f,     0.8f, 0.8f, 0.3f,  0.f,  0.f},   //Left low
            Vertex{ 0.25f, -0.25f,  0.25f,     0.8f, 0.8f, 0.3f,  1.f,  0.f},   //Right low
            Vertex{ 0.0f,   0.25f,  0.0f,      0.8f, 0.8f, 0.3f,  0.5f, 0.5f},  //Top
            Vertex{ 0.0f,  -0.25f, -0.25f,     0.8f, 0.8f, 0.3f,  0.5f, 0.5f}   //Back low
        }
    );

    mIndices.insert( mIndices.end(),
        {
            0, 1, 2,
            1, 3, 2,
            3, 0, 2,
            0, 3, 1
        }
    );

    mPosition = {1.5, 1.0, 3.0};
    mMatrix.translate(mPosition.x(), mPosition.y(), mPosition.z());
    mColor = { mLightColor[0], mLightColor[1], mLightColor[2] };

    drawType = 0; // 0 = fill color, 1 = line color, 2 = fill phong
}

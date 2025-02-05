#include "spiralgenerator.h"

SpiralGenerator::SpiralGenerator() {}


SpiralGenerator::SpiralGenerator(const float minZ, const float maxZ, const int Steps)
{
    float z = minZ;

    for (int s = 0; s < Steps; s++)
    {
        z += (maxZ - minZ) / Steps;

        mVertices.push_back({cos(z)/10,z/25,sin(z)/10,cos(z), z/(maxZ - minZ), sin(z),0,0});
    }

}

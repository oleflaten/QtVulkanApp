#ifndef SPIRALGENERATOR_H
#define SPIRALGENERATOR_H

#include "visualobject.h"

class SpiralGenerator : public VisualObject
{
public:
    SpiralGenerator();
    SpiralGenerator(const float minZ, const float maxZ, const int Steps);

};

#endif // SPIRALGENERATOR_H

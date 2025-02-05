#ifndef APESADELGENERATOR_H
#define APESADELGENERATOR_H

#include "visualobject.h"

class ApeSadelGenerator : public VisualObject
{
public:
    ApeSadelGenerator();
    ApeSadelGenerator(const float minValue, const float maxValue, const float h);
};

#endif // APESADELGENERATOR_H

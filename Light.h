#ifndef LIGHT_H
#define LIGHT_H

#include "VisualObject.h"

class Light : public VisualObject
{
public:
    Light();

    float mAmbientStrenght{0.3f};
    float mAmbientColor[3]{0.3f, 0.3f, 0.3f};  //Grey

    float mLightStrenght{0.7f};
    float mLightColor[3]{0.9f, 0.9f, 0.3f};    //Yellow

    float mSpecularStrenght{0.9f};
    int mSpecularExponent{4};
};

#endif // LIGHT_H

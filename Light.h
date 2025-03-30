#ifndef LIGHT_H
#define LIGHT_H

#include "VisualObject.h"

class Light : public VisualObject
{
public:
    Light();

    float mAmbientStrenght{0.3f};
    float mAmbientColor[3]{0.3f, 1.0f, 0.3f};  //Green

    float mLightStrenght{1.0f};
    float mLightColor[3]{0.9f, 0.9f, 0.3f};    //Yellow

    float mSpecularStrenght{0.9f};
    int mSpecularExponent{32};
};

#endif // LIGHT_H

#ifndef VKVERTEX_H
#define VKVERTEX_H

#include "iostream"

class VKVertex
{
public:
    VKVertex();
    VKVertex(float setX, float setY, float setZ, float setR, float setG, float setB, float setU = 0.f, float setV = 0.f);

    float x,y,z,r,g,b,u,v;

    friend std::ostream& operator<< (std::ostream&, const VKVertex&);

    friend std::istream& operator>> (std::istream&, VKVertex&);
};

#endif // VKVERTEX_H

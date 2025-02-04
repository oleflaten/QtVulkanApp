#ifndef VKVERTEX_H
#define VKVERTEX_H

class VKVertex
{
public:
    VKVertex();
    VKVertex(float setX, float setY, float setZ, float setR, float setG, float setB, float setU = 0.f, float setV = 0.f);

    float x,y,z,r,g,b,u,v;
};

#endif // VKVERTEX_H

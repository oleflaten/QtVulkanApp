#include "vkvertex.h"

VKVertex::VKVertex() {}

VKVertex::VKVertex(float newX, float newY, float newZ, float newR, float newG, float newB, float newU, float newV)
{
    x = newX;
    y = newY;
    z = newZ;
    r = newR;
    g = newG;
    b = newB;
    u = newU;
    v = newV;
}


std::ostream& operator<< (std::ostream& os, const VKVertex& v) {
    os << std::fixed;
    os << "(" << v.x << ", " << v.y << ", " << v.z << ") ";
    os << "(" << v.r << ", " << v.g << ", " << v.b << ") ";
    os << "(" << v.u << ", " << v.v << ") ";
    return os;
}
std::istream& operator>> (std::istream& is, VKVertex& v) {
    // Trenger fire temporære variabler som kun skal lese inn parenteser og komma
    char dum, dum2, dum3, dum4;
    is >> dum >> v.x >> dum2 >> v.y >> dum3 >> v.z >> dum4;
    is >> dum >> v.r >> dum2 >> v.g >> dum3 >> v.b >> dum4;
    is >> dum >> v.u >> dum2 >> v.v >> dum3;
    return is;
}

#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "VisualObject.h"

class BezierCurve : public VisualObject
{
private:
    int mDimension;

public:
    BezierCurve(int dimension = 3);

    QVector3D CalculateInterpolation(float t);
};

#endif // BEZIERCURVE_H

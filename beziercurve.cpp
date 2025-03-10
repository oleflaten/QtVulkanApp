#include "beziercurve.h"

BezierCurve::BezierCurve(int dimension)
{
    mDimension = dimension;
}


QVector3D BezierCurve::CalculateInterpolation(float t)
{
    std::vector<QVector3D> a = {};

    for (int i = 0; i < mVertices.size(); i++)
    {
        a.push_back({mVertices[i].x,mVertices[i].y,mVertices[i].z});
    }

    for (int k = mDimension; k > 0; k--)
    {
        for (int i = 0; i < k; i++)
        {
            a[i] = a[i] * (1 - t) + a[i + 1] * t;
        }
    }

    return a[1];
}

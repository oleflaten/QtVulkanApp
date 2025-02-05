#include "apesadelgenerator.h"

ApeSadelGenerator::ApeSadelGenerator() {}


ApeSadelGenerator::ApeSadelGenerator(const float minValue, const float maxValue, const float h)
{
    float reps = (maxValue - minValue)/h;

    std::vector<VKVertex> tempVertices;

    //Generates all vertices for the "apesadel"
    for (float x = minValue; x <= maxValue; x += h)
    {
        for (float y = minValue; y <= maxValue; y += h)
        {
            tempVertices.push_back({x,x*x*y,y,x,x*x*y,y,0,0});
        }
    }

    //Generates all vertices for the triangle mesh for the "apesadel"

    for (int x = 0; x <= reps; x++)
    {

        for (int y = 0; y <= reps; y++)
        {
            //Adds bottom triangle
            if (x - 1 >= 0 && y + 1 <= reps)
            {
                mVertices.push_back({tempVertices[(x - 1) * (reps + 1) + y + 1]});
                mVertices.push_back({tempVertices[(x - 1) * (reps + 1) + y]});
                mVertices.push_back({tempVertices[(x) * (reps + 1) + y]});
            }

            //Adds top triangle
            if (x - 1 >= 0 && y - 1 >= 0)
            {
                mVertices.push_back({tempVertices[(x - 1) * (reps + 1) + y]});
                mVertices.push_back({tempVertices[(x) * (reps + 1) + y - 1]});
                mVertices.push_back({tempVertices[(x) * (reps + 1) + y]});
            }
        }
    }
}

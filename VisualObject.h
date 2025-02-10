#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QVulkanWindow>
#include <vector>
#include "vertex.h"

class VisualObject
{
public:
    std::vector<Vertex> mVertices;
    std::vector<Vertex> getVertices() { return mVertices; }
    VisualObject();
    void setName(std::string name);
    std::string getName() const;

    //
    VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
    VkBuffer mBuffer{ VK_NULL_HANDLE };
    VkPrimitiveTopology mTopology { VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
    //
    QMatrix4x4 mMatrix;
    void move(float x, float y=0.0f, float z=0.0f);
    void scale(float s);
    void rotate(float t, float x, float y, float z);
protected:
    std::string mName;
};

#endif // VISUALOBJECT_H


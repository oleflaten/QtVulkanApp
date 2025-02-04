#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H


#include <QVulkanWindow> 	// VkDeviceMemory, VkBuffer defined here
#include <vector>
#include "vkvertex.h"


class VisualObject
{
public:
    std::vector<VKVertex> mVertices;
    std::vector<VKVertex> getVertices() { return mVertices; }
    VisualObject();
    //
    VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
    VkBuffer mBuffer{ VK_NULL_HANDLE };
    VkPrimitiveTopology mTopology { VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
    //
    QMatrix4x4 mMatrix;
};

#endif

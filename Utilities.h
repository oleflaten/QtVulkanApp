#ifndef UTILITIES_H
#define UTILITIES_H

#include <QVulkanFunctions>
#include <QMatrix4x4>
#include <string>

//Utility function for alignment:
static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

//Utility struct for handling buffers
struct BufferHandle
{
    VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
    VkBuffer mBuffer{ VK_NULL_HANDLE };
};

struct TextureHandle
{
	VkDeviceMemory mTextureMemory{ VK_NULL_HANDLE };
	VkImage mImage{ VK_NULL_HANDLE };
	VkImageView mImageView{ VK_NULL_HANDLE };
	VkDescriptorSet mTextureDescriptorSet{ VK_NULL_HANDLE };
};

static const std::string assetPath = "../../Assets/";

#endif // UTILITIES_H

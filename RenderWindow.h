#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QVulkanWindow>

class RenderWindow : public QVulkanWindowRenderer
{
public:
    RenderWindow(QVulkanWindow *w, bool msaa = false);

    //Initializes the Vulkan resources needed,
    // the buffers
    // vertex descriptions for the shaders
    // making the shaders, etc
    void initResources() override;

    //Set up resources - only MVP-matrix for now:
    void initSwapChainResources() override;

    //Empty for now - needed since we implement QVulkanWindowRenderer
    void releaseSwapChainResources() override;

    //Release Vulkan resources when program ends
    //Called by Qt
    void releaseResources() override;

    //Render the next frame
    void startNextFrame() override;

    //Get Vulkan info - just for fun
    void getVulkanHWInfo();

protected:

    //Creates the Vulkan shader module from the precompiled shader files in .spv format
    VkShaderModule createShader(const QString &name);

    //The ModelViewProjection MVP matrix
    QMatrix4x4 mProj;
    //Rotation angle of the triangle
    float mRotation{ 0.0f };

    //Test variable to move the triangle
    float mMove{ 0.7f };

    //Vulkan resources:
    QVulkanWindow* mWindow{ nullptr };
    QVulkanDeviceFunctions *mDeviceFunctions{ nullptr };

    VkDeviceMemory mBufMem = VK_NULL_HANDLE;
    VkBuffer mBuf = VK_NULL_HANDLE;
    VkDescriptorBufferInfo mUniformBufInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    VkDescriptorPool mDescPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout mDescSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet mDescSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;

private:
    void updateUniformBuffer(const QMatrix4x4& modelMatrix, int currentFrame);
};

#endif // RENDERWINDOW_H

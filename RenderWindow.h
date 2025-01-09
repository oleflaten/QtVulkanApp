#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QVulkanWindow>

class RenderWindow : public QVulkanWindowRenderer
{
public:
    RenderWindow(QVulkanWindow *w, bool msaa = false);

    //Initializes the Vulkan resources needed,
    // for instance the buffers
    // vertex descriptions for the shaders
    // making the shaders
    void initResources() override;

    //Set up resources - only MVP-matrix for now:
    void initSwapChainResources() override;

    //Empty for now
    void releaseSwapChainResources() override;

    //Release Vulkan resources when program ends
    //Called by Qt
    void releaseResources() override;

    //Render the next frame
    void startNextFrame() override;

protected:

    //Creates the Vulkan shader module from the precompiled shader files in .spv format
    VkShaderModule createShader(const QString &name);

    //The MVP matrix
    QMatrix4x4 m_proj;
    //Rotation angle of the triangle
    float m_rotation{ 0.0f };

    //Vulkan resources:
    QVulkanWindow* m_window{ nullptr };
    QVulkanDeviceFunctions *m_deviceFunctions{ nullptr };

    VkDeviceMemory m_bufMem = VK_NULL_HANDLE;
    VkBuffer m_buf = VK_NULL_HANDLE;
    VkDescriptorBufferInfo m_uniformBufInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    VkDescriptorPool m_descPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet m_descSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};

#endif // RENDERWINDOW_H

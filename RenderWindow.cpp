#include "RenderWindow.h"
#include <QVulkanFunctions>
#include <QFile>

// Vertex data and the projection matrix assume OpenGL's clip space.
// Vulkan Y is negated in clip space and the near/far plane is at 0/1 instead
// of -1/1. These will be corrected for by an extra transformation when
// calculating the modelview-projection matrix.
static float vertexData[] = {
    // Y up, front = CCW
    // X,     Y,     Z,     R,    G,    B
    0.0f,   0.5f,  0.0f,  1.0f, 0.0f, 0.0f,
    -0.5f,  -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,
    0.5f,  -0.5f,  0.0f,  0.0f, 0.0f, 1.0f
};

static const int UNIFORM_DATA_SIZE = 16 * sizeof(float);

static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

RenderWindow::RenderWindow(QVulkanWindow *w, bool msaa)
    : m_window(w)
{
    if (msaa) {
        const QList<int> counts = w->supportedSampleCounts();
        qDebug() << "Supported sample counts:" << counts;
        for (int s = 16; s >= 4; s /= 2) {
            if (counts.contains(s)) {
                qDebug("Requesting sample count %d", s);
                m_window->setSampleCount(s);
                break;
            }
        }
    }
}

VkShaderModule RenderWindow::createShader(const QString &name)
{
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to read shader %s", qPrintable(name));
        return VK_NULL_HANDLE;
    }
    QByteArray blob = file.readAll();
    file.close();

    VkShaderModuleCreateInfo shaderInfo;
    memset(&shaderInfo, 0, sizeof(shaderInfo));
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = blob.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
    VkShaderModule shaderModule;
    VkResult err = m_deviceFunctions->vkCreateShaderModule(m_window->device(), &shaderInfo, nullptr, &shaderModule);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create shader module: %d", err);
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

void RenderWindow::initResources()
{
    qDebug("initResources");

    VkDevice dev = m_window->device();
    m_deviceFunctions = m_window->vulkanInstance()->deviceFunctions(dev);

    /* Prepare the vertex and uniform data.The vertex data will never
    change so one buffer is sufficient regardless of the value of
    QVulkanWindow::CONCURRENT_FRAME_COUNT. Uniform data is changing per
    frame however so active frames have to have a dedicated copy.

    Use just one memory allocation and one buffer. We will then specify the
    appropriate offsets for uniform buffers in the VkDescriptorBufferInfo.
    Have to watch out for
    VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment, though.

    The uniform buffer is not strictly required in this example, we could
    have used push constants as well since our single matrix (64 bytes) fits
    into the spec mandated minimum limit of 128 bytes. However, once that
    limit is not sufficient, the per-frame buffers, as shown below, will
    become necessary.
    */
    const int concurrentFrameCount = m_window->concurrentFrameCount();
    const VkPhysicalDeviceLimits *pdevLimits = &m_window->physicalDeviceProperties()->limits;
    const VkDeviceSize uniAlign = pdevLimits->minUniformBufferOffsetAlignment;
    qDebug("uniform buffer offset alignment is %u", (uint) uniAlign);
    VkBufferCreateInfo bufInfo;
    memset(&bufInfo, 0, sizeof(bufInfo));
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    // Our internal layout is vertex, uniform, uniform, ... with each uniform buffer start offset aligned to uniAlign.
    const VkDeviceSize vertexAllocSize = aligned(sizeof(vertexData), uniAlign);
    const VkDeviceSize uniformAllocSize = aligned(UNIFORM_DATA_SIZE, uniAlign);
    bufInfo.size = vertexAllocSize + concurrentFrameCount * uniformAllocSize;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    VkResult err = m_deviceFunctions->vkCreateBuffer(dev, &bufInfo, nullptr, &m_buf);
    if (err != VK_SUCCESS)
        qFatal("Failed to create buffer: %d", err);

    VkMemoryRequirements memReq;
    m_deviceFunctions->vkGetBufferMemoryRequirements(dev, m_buf, &memReq);

    VkMemoryAllocateInfo memAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        memReq.size,
        m_window->hostVisibleMemoryIndex()
    };

    err = m_deviceFunctions->vkAllocateMemory(dev, &memAllocInfo, nullptr, &m_bufMem);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate memory: %d", err);

    err = m_deviceFunctions->vkBindBufferMemory(dev, m_buf, m_bufMem, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind buffer memory: %d", err);

    quint8 *p;
    err = m_deviceFunctions->vkMapMemory(dev, m_bufMem, 0, memReq.size, 0, reinterpret_cast<void **>(&p));
    if (err != VK_SUCCESS)
        qFatal("Failed to map memory: %d", err);
    memcpy(p, vertexData, sizeof(vertexData));
    QMatrix4x4 ident;
    memset(m_uniformBufInfo, 0, sizeof(m_uniformBufInfo));
    for (int i = 0; i < concurrentFrameCount; ++i) {
        const VkDeviceSize offset = vertexAllocSize + i * uniformAllocSize;
        memcpy(p + offset, ident.constData(), 16 * sizeof(float));
        m_uniformBufInfo[i].buffer = m_buf;
        m_uniformBufInfo[i].offset = offset;
        m_uniformBufInfo[i].range = uniformAllocSize;
    }
    m_deviceFunctions->vkUnmapMemory(dev, m_bufMem);


    /********************************* Vertex layout: *********************************/

    //The size of each vertex to be passed to the shader
    VkVertexInputBindingDescription vertexBindingDesc = {
        0, // binding - has to match that in VkVertexInputAttributeDescription and startNextFrame()s m_devFuncs->vkCmdBindVertexBuffers
        6 * sizeof(float), // stride account for X, Y, Z, R, G, B
        VK_VERTEX_INPUT_RATE_VERTEX
    };

    /********************************* Shader bindings: *********************************/
    //Descritpion of the attributes used in the shader
    VkVertexInputAttributeDescription vertexAttrDesc[] = {
        { // position
            0, // location has to correspond to the layout(location = x) in the shader
            0, // binding
            VK_FORMAT_R32G32B32_SFLOAT, // updated format to include Z
            0
        },
        { // color
            1, // location has to correspond to the layout(location = x) in the shader
            0, // binding
            VK_FORMAT_R32G32B32_SFLOAT,
            3 * sizeof(float) // offset to account for X, Y, Z
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;

    // Set up descriptor set and its layout.
    VkDescriptorPoolSize descPoolSizes = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uint32_t(concurrentFrameCount) };
    VkDescriptorPoolCreateInfo descPoolInfo;
    memset(&descPoolInfo, 0, sizeof(descPoolInfo));
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.maxSets = concurrentFrameCount;
    descPoolInfo.poolSizeCount = 1;
    descPoolInfo.pPoolSizes = &descPoolSizes;
    err = m_deviceFunctions->vkCreateDescriptorPool(dev, &descPoolInfo, nullptr, &m_descPool);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor pool: %d", err);

    VkDescriptorSetLayoutBinding layoutBinding = {
        0, // binding
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        1,
        VK_SHADER_STAGE_VERTEX_BIT,
        nullptr
    };
    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        1,
        &layoutBinding
    };
    err = m_deviceFunctions->vkCreateDescriptorSetLayout(dev, &descLayoutInfo, nullptr, &m_descSetLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor set layout: %d", err);

    for (int i = 0; i < concurrentFrameCount; ++i) {
        VkDescriptorSetAllocateInfo descSetAllocInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            nullptr,
            m_descPool,
            1,
            &m_descSetLayout
        };
        err = m_deviceFunctions->vkAllocateDescriptorSets(dev, &descSetAllocInfo, &m_descSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate descriptor set: %d", err);

        VkWriteDescriptorSet descWrite;
        memset(&descWrite, 0, sizeof(descWrite));
        descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite.dstSet = m_descSet[i];
        descWrite.descriptorCount = 1;
        descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descWrite.pBufferInfo = &m_uniformBufInfo[i];
        m_deviceFunctions->vkUpdateDescriptorSets(dev, 1, &descWrite, 0, nullptr);
    }

    // Pipeline cache
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    err = m_deviceFunctions->vkCreatePipelineCache(dev, &pipelineCacheInfo, nullptr, &m_pipelineCache);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline cache: %d", err);

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descSetLayout;
    err = m_deviceFunctions->vkCreatePipelineLayout(dev, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline layout: %d", err);

    /********************************* Create shaders *********************************/
    //Creates our actuall shader modules
    VkShaderModule vertShaderModule = createShader(QStringLiteral(":/color_vert.spv"));
    VkShaderModule fragShaderModule = createShader(QStringLiteral(":/color_frag.spv"));

    // Graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo;
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_VERTEX_BIT,
            vertShaderModule,
            "main",
            nullptr
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            fragShaderModule,
            "main",
            nullptr
        }
    };
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;

    VkPipelineInputAssemblyStateCreateInfo ia;
    memset(&ia, 0, sizeof(ia));
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInfo.pInputAssemblyState = &ia;

    // The viewport and scissor will be set dynamically via vkCmdSetViewport/Scissor.
    // This way the pipeline does not need to be touched when resizing the window.
    VkPipelineViewportStateCreateInfo vp;
    memset(&vp, 0, sizeof(vp));
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount = 1;
    pipelineInfo.pViewportState = &vp;

    VkPipelineRasterizationStateCreateInfo rs;
    memset(&rs, 0, sizeof(rs));
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_NONE; // we want the back face as well
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.lineWidth = 1.0f;
    pipelineInfo.pRasterizationState = &rs;

    VkPipelineMultisampleStateCreateInfo ms;
    memset(&ms, 0, sizeof(ms));
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // Enable multisampling.
    ms.rasterizationSamples = m_window->sampleCountFlagBits();
    pipelineInfo.pMultisampleState = &ms;

    VkPipelineDepthStencilStateCreateInfo ds;
    memset(&ds, 0, sizeof(ds));
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pipelineInfo.pDepthStencilState = &ds;

    VkPipelineColorBlendStateCreateInfo cb;
    memset(&cb, 0, sizeof(cb));
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // no blend, write out all of rgba
    VkPipelineColorBlendAttachmentState att;
    memset(&att, 0, sizeof(att));
    att.colorWriteMask = 0xF;
    cb.attachmentCount = 1;
    cb.pAttachments = &att;
    pipelineInfo.pColorBlendState = &cb;

    VkDynamicState dynEnable[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dyn;
    memset(&dyn, 0, sizeof(dyn));
    dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn.dynamicStateCount = sizeof(dynEnable) / sizeof(VkDynamicState);
    dyn.pDynamicStates = dynEnable;
    pipelineInfo.pDynamicState = &dyn;

    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_window->defaultRenderPass();

    err = m_deviceFunctions->vkCreateGraphicsPipelines(dev, m_pipelineCache, 1, &pipelineInfo, nullptr, &m_pipeline);
    if (err != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", err);

    if (vertShaderModule)
        m_deviceFunctions->vkDestroyShaderModule(dev, vertShaderModule, nullptr);
    if (fragShaderModule)
        m_deviceFunctions->vkDestroyShaderModule(dev, fragShaderModule, nullptr);
}

void RenderWindow::initSwapChainResources()
{
    qDebug("initSwapChainResources");

    // Projection matrix
    m_proj = m_window->clipCorrectionMatrix(); // adjust for Vulkan-OpenGL clip space differences
    const QSize sz = m_window->swapChainImageSize();
    m_proj.perspective(45.0f, sz.width() / (float) sz.height(), 0.01f, 100.0f);
    m_proj.translate(0, 0, -4);
}

void RenderWindow::releaseSwapChainResources()
{
    qDebug("releaseSwapChainResources");
}

void RenderWindow::releaseResources()
{
    qDebug("releaseResources");

    VkDevice dev = m_window->device();

    if (m_pipeline) {
        m_deviceFunctions->vkDestroyPipeline(dev, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout) {
        m_deviceFunctions->vkDestroyPipelineLayout(dev, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }

    if (m_pipelineCache) {
        m_deviceFunctions->vkDestroyPipelineCache(dev, m_pipelineCache, nullptr);
        m_pipelineCache = VK_NULL_HANDLE;
    }

    if (m_descSetLayout) {
        m_deviceFunctions->vkDestroyDescriptorSetLayout(dev, m_descSetLayout, nullptr);
        m_descSetLayout = VK_NULL_HANDLE;
    }

    if (m_descPool) {
        m_deviceFunctions->vkDestroyDescriptorPool(dev, m_descPool, nullptr);
        m_descPool = VK_NULL_HANDLE;
    }

    if (m_buf) {
        m_deviceFunctions->vkDestroyBuffer(dev, m_buf, nullptr);
        m_buf = VK_NULL_HANDLE;
    }

    if (m_bufMem) {
        m_deviceFunctions->vkFreeMemory(dev, m_bufMem, nullptr);
        m_bufMem = VK_NULL_HANDLE;
    }
}

void RenderWindow::startNextFrame()
{
    VkDevice dev = m_window->device();
    VkCommandBuffer cb = m_window->currentCommandBuffer();
    const QSize sz = m_window->swapChainImageSize();

    //Backtgound color of the render window - dark grey
    VkClearColorValue clearColor = {{ 0.3, 0.3, 0.3, 1 }};

    VkClearDepthStencilValue clearDS = { 1, 0 };
    VkClearValue clearValues[3];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearValues[2].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = m_window->defaultRenderPass();
    rpBeginInfo.framebuffer = m_window->currentFramebuffer();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = m_window->sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
    rpBeginInfo.pClearValues = clearValues;
    VkCommandBuffer cmdBuf = m_window->currentCommandBuffer();
    m_deviceFunctions->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    quint8* GPUmemPointer;  //for vertex and uniform data
    VkResult err = m_deviceFunctions->vkMapMemory(dev, m_bufMem, m_uniformBufInfo[m_window->currentFrame()].offset,
                                                  UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
    if (err != VK_SUCCESS)
        qFatal("Failed to map memory: %d", err);

    /********************************* Set the rotation in our matrix *********************************/
    QMatrix4x4 tempMatrix = m_proj;
    tempMatrix.rotate(m_rotation, 0, 1, 0);
    memcpy(GPUmemPointer, tempMatrix.constData(), 16 * sizeof(float));
    m_deviceFunctions->vkUnmapMemory(dev, m_bufMem);

    //rotate the triangle 1 degree per frame
    m_rotation += 1.0f;

    m_deviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    m_deviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
                                               &m_descSet[m_window->currentFrame()], 0, nullptr);
    VkDeviceSize vbOffset = 0;

    //The second parameter here is the binding to the VertexInputBindingDescription,
    //so it has to be the same number used there
    m_deviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &m_buf, &vbOffset);

    VkViewport viewport;
    viewport.x = viewport.y = 0;
    viewport.width = sz.width();
    viewport.height = sz.height();
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    m_deviceFunctions->vkCmdSetViewport(cb, 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = viewport.width;
    scissor.extent.height = viewport.height;
    m_deviceFunctions->vkCmdSetScissor(cb, 0, 1, &scissor);

    /********************************* Our draw call!: *********************************/
    m_deviceFunctions->vkCmdDraw(cb, 3, 1, 0, 0);

    m_deviceFunctions->vkCmdEndRenderPass(cmdBuf);

    /*QVulkanWindow subclasses queue their draw calls in their reimplementation of
    QVulkanWindowRenderer::startNextFrame(). Once done, they are required to call back
    QVulkanWindow::frameReady(). The example has no asynchronous command generation, so the
    frameReady() call is made directly from startNextFrame().
    To get continuous updates, the example simply invokes QWindow::requestUpdate() in order to schedule a repaint.
    This means that it requests the Qt window system to call the update() method,
    which will eventually lead to the paintEvent() being called.
    */
    m_window->frameReady();
    m_window->requestUpdate(); // render continuously, throttled by the presentation rate
}

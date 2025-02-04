#include "visualobject.h"
#include "RenderWindow.h"
#include <QVulkanFunctions>
#include <QFile>


// Hardcoded mesh for now. Will be put in its own class soon!
// NB 1: Vulkan's near/far plane (Z axis) is at 0/1 instead of -1/1, as in OpenGL!
// NB 2: Vulkan Y is negated in clip space so we fix that when making the projection matrix
// **PLAY WITH THIS**
static float vertexData[] = {
    // Y up, front = CCW
    // X,     Y,     Z,     R,    G,    B
    0.0f,   0.5f,  0.0f,   1.0f, 0.0f, 0.0f,    //top vertex - red
    -0.5f,  -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,    //bottom left vertex - green
    0.5f,  -0.5f,  0.0f,   0.0f, 0.0f, 1.0f     //bottom right vertex - blue
};

//Utility function for alignment:
static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}


void RenderWindow::createBuffer(VkDevice logicalDevice, const VkDeviceSize uniAlign, VisualObject* visualObject, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo bufferInfo{};
    memset(&bufferInfo, 0, sizeof(bufferInfo)); //Clear out the memory
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; // Set the structure type


    // Layout is just the vertex data
    // start offset aligned to uniAlign.


    VkDeviceSize vertexAllocSize = aligned(visualObject->getVertices().size()*sizeof(VKVertex), uniAlign);
    bufferInfo.size = vertexAllocSize; //One vertex buffer (we don't use Uniform buffer in this example)
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // Set the usage vertex buffer (not using Uniform buffer in this example)

    VkResult err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &visualObject->mBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create buffer: %d", err);


    VkMemoryRequirements memReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, visualObject->mBuffer, &memReq);


    VkMemoryAllocateInfo memAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        memReq.size,
        mWindow->hostVisibleMemoryIndex()
    };


    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &memAllocInfo, nullptr, &visualObject->mBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate memory: %d", err);


    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, visualObject->mBuffer, visualObject->mBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind buffer memory: %d", err);


    quint8* p{nullptr};
    err = mDeviceFunctions->vkMapMemory(logicalDevice, visualObject->mBufferMemory, 0, memReq.size, 0, reinterpret_cast<void **>(&p));
    if (err != VK_SUCCESS)
        qFatal("Failed to map memory: %d", err);
    // Dag 170125
    // memcpy(p, vertexData, sizeof(vertexData));
    memcpy(p, visualObject->getVertices().data(), visualObject->getVertices().size()*sizeof(VKVertex));


    mDeviceFunctions->vkUnmapMemory(logicalDevice, visualObject->mBufferMemory);
}


/*** RenderWindow class ***/

RenderWindow::RenderWindow(QVulkanWindow *w, bool msaa)
	: mWindow(w)
{
    //mObjects.push_back(new VkTriangle());
    mObjects.push_back(new VkTriangleSurface("C:/Users/bjorn/Documents/GitHub/Vulkan/QtVulkanApp/vertices_1.txt"));
}


void RenderWindow::initResources()
{
    qDebug("\n ***************************** initResources ******************************************* \n");

    VkDevice logicalDevice = mWindow->device();
    mDeviceFunctions = mWindow->vulkanInstance()->deviceFunctions(logicalDevice);

    /* Prepare the vertex and uniform data.The vertex data will never
    change so one buffer is sufficient regardless of the value of
    QVulkanWindow::CONCURRENT_FRAME_COUNT. */

    const int concurrentFrameCount = mWindow->concurrentFrameCount(); // 2 on Oles Machine
    const VkPhysicalDeviceLimits *pdevLimits = &mWindow->physicalDeviceProperties()->limits;
    const VkDeviceSize uniAlign = pdevLimits->minUniformBufferOffsetAlignment;
	qDebug("uniform buffer offset alignment is %u", (uint)uniAlign); //64 on Oles machine

    VkBufferCreateInfo bufferInfo{};
    memset(&bufferInfo, 0, sizeof(bufferInfo));
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; // Set the structure type


    for (auto it : mObjects)
    {
        createBuffer(logicalDevice, uniAlign, it);

        qDebug("YESYESYES");
    }


    /********************************* Vertex layout: *********************************/

    //The size of each vertex to be passed to the shader
    VkVertexInputBindingDescription vertexBindingDesc = {
        0, // binding - has to match that in VkVertexInputAttributeDescription and startNextFrame()s m_devFuncs->vkCmdBindVertexBuffers
        sizeof(VKVertex), // stride account for X, Y, Z, R, G, B
        VK_VERTEX_INPUT_RATE_VERTEX
    };

    /********************************* Shader bindings: *********************************/
    //Descritpion of the attributes used for vertices in the shader
    VkVertexInputAttributeDescription vertexAttrDesc[] = {
        { // position
            0, // location has to correspond to the layout(location = x) in the shader
            0, // binding
            VK_FORMAT_R32G32B32_SFLOAT,
            0
        },
        { // color
            1, // location has to correspond to the layout(location = x) in the shader
            0, // binding
            VK_FORMAT_R32G32B32_SFLOAT,
            3 * sizeof(float) // offset to account for X, Y, Z
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount = 2; // position and color
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;

	// Pipeline cache - supposed to increase performance
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VkResult err = mDeviceFunctions->vkCreatePipelineCache(logicalDevice, &pipelineCacheInfo, nullptr, &mPipelineCache);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline cache: %d", err);

    // Pipeline layout
    
    // OEF: PushConstants update - set up the push constant info
    VkPushConstantRange pushConstantRange{
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
		16 * sizeof(float) // 16 floats for the model matrix
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 1;  // OEF: PushConstants update
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // OEF: PushConstants update
    err = mDeviceFunctions->vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
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

    pipelineInfo.stageCount = 2; //vertex and fragment shader
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    VkPipelineInputAssemblyStateCreateInfo ia;
    memset(&ia, 0, sizeof(ia));
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
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
    ms.rasterizationSamples = mWindow->sampleCountFlagBits();
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

    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = mWindow->defaultRenderPass();

    err = mDeviceFunctions->vkCreateGraphicsPipelines(logicalDevice, mPipelineCache, 1, &pipelineInfo, nullptr, &mPipeline);
    if (err != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", err);

    if (vertShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
    if (fragShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);

    qDebug("\n ***************************** initResources finished ******************************************* \n");

    getVulkanHWInfo(); // if you want to get info about the Vulkan hardware
}

void RenderWindow::initSwapChainResources()
{
    qDebug("\n ***************************** initSwapChainResources ******************************************* \n");

    // Projection matrix - how the scene will be projected into the render window

    //This function is called at startup and when the app window is resized
    mProjectionMatrix.setToIdentity();
    //find the size of the window
    const QSize sz = mWindow->swapChainImageSize();

    //               vertical angle ,   aspect ratio                    near-  , far plane
    /**PLAY WITH THIS**/
    mProjectionMatrix.perspective(25.0f,          sz.width() / (float) sz.height(), 0.01f, 100.0f);
    //Camera is -4 away from origo
    /**PLAY WITH THIS**/
    mProjectionMatrix.translate(0, 0, -4);

    //Flip projection because of Vulkan's -Y axis
    mProjectionMatrix.scale(1.0f, -1.0f, 1.0);

    mProjectionMatrix.rotate(0.f,0.f,0.f);
}

void RenderWindow::startNextFrame()
{
    VkDevice dev = mWindow->device();
    VkCommandBuffer cmdBuf = mWindow->currentCommandBuffer();
    const QSize sz = mWindow->swapChainImageSize();

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
    rpBeginInfo.renderPass = mWindow->defaultRenderPass();
    rpBeginInfo.framebuffer = mWindow->currentFramebuffer();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = mWindow->sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
    rpBeginInfo.pClearValues = clearValues;
    mDeviceFunctions->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    mDeviceFunctions->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
 
    VkDeviceSize vbOffset = 0;

    VkViewport viewport{};
    viewport.x = viewport.y = 0;
    viewport.width = sz.width();
    viewport.height = sz.height();
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    mDeviceFunctions->vkCmdSetViewport(cmdBuf, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = viewport.width;
    scissor.extent.height = viewport.height;
    mDeviceFunctions->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

    /********************************* Our draw call!: *********************************/

	//Set model matrix for first triangle
    //We make a temp of this to not mess up the original matrix
    //QMatrix4x4 tempMatrix = mProjectionMatrix;
    //tempMatrix.translate(-0.7f, 0, 0);
    //tempMatrix.rotate(mRotation, 0, 1, 0);

    for (auto it : mObjects)
    {
        mDeviceFunctions->vkCmdBindVertexBuffers(cmdBuf, 0, 1, &(it)->mBuffer, &vbOffset);
        setModelMatrix(mProjectionMatrix * (it)->mMatrix);
        mDeviceFunctions->vkCmdDraw(cmdBuf, (it)->mVertices.size(), 1, 0, 0);

        qDebug("NONONONONONONONONONO");
        qDebug("WAITWAITWAITWAITWAIT");
    }
/*
    //Push the model matrix to the shader and draw the triangle
    setModelMatrix(tempMatrix);
    mDeviceFunctions->vkCmdDraw(cmdBuf, 3, 1, 0, 0);

    //Set model matrix for second triangle
    tempMatrix = mProjectionMatrix;
    tempMatrix.translate(0.7f, 0, 0);
    tempMatrix.rotate(mRotation, 0, 0, 1);

    setModelMatrix(tempMatrix);
    mDeviceFunctions->vkCmdDraw(cmdBuf, 3, 1, 0, 0);
*/

    mDeviceFunctions->vkCmdEndRenderPass(cmdBuf);

    mWindow->frameReady();
    mWindow->requestUpdate(); // render continuously, throttled by the presentation rate

    mRotation += 1.0f; //set for next frame
}

VkShaderModule RenderWindow::createShader(const QString &name)
{
    //This uses Qt's own file opening and resource system
    //We probably will replace it with pure C++ when expanding the program
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
    VkResult err = mDeviceFunctions->vkCreateShaderModule(mWindow->device(), &shaderInfo, nullptr, &shaderModule);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create shader module: %d", err);
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

void RenderWindow::setModelMatrix(QMatrix4x4 modelMatrix)
{

	mDeviceFunctions->vkCmdPushConstants(mWindow->currentCommandBuffer(), mPipelineLayout, 
        VK_SHADER_STAGE_VERTEX_BIT, 0, 16 * sizeof(float), modelMatrix.constData());
}

void RenderWindow::getVulkanHWInfo()
{
    qDebug("\n ***************************** Vulkan Hardware Info ******************************************* \n");
    QVulkanInstance *inst = mWindow->vulkanInstance();
    mDeviceFunctions = inst->deviceFunctions(mWindow->device());

    QString info;
    info += QString::asprintf("Number of physical devices: %d\n", int(mWindow->availablePhysicalDevices().count()));

    QVulkanFunctions *f = inst->functions();
    VkPhysicalDeviceProperties props;
    f->vkGetPhysicalDeviceProperties(mWindow->physicalDevice(), &props);
    info += QString::asprintf("Active physical device name: '%s' version %d.%d.%d\nAPI version %d.%d.%d\n",
                              props.deviceName,
                              VK_VERSION_MAJOR(props.driverVersion), VK_VERSION_MINOR(props.driverVersion),
                              VK_VERSION_PATCH(props.driverVersion),
                              VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion),
                              VK_VERSION_PATCH(props.apiVersion));

    info += QStringLiteral("Supported instance layers:\n");
    for (const QVulkanLayer &layer : inst->supportedLayers())
        info += QString::asprintf("    %s v%u\n", layer.name.constData(), layer.version);
    info += QStringLiteral("Enabled instance layers:\n");
    for (const QByteArray &layer : inst->layers())
        info += QString::asprintf("    %s\n", layer.constData());

    info += QStringLiteral("Supported instance extensions:\n");
    for (const QVulkanExtension &ext : inst->supportedExtensions())
        info += QString::asprintf("    %s v%u\n", ext.name.constData(), ext.version);
    info += QStringLiteral("Enabled instance extensions:\n");
    for (const QByteArray &ext : inst->extensions())
        info += QString::asprintf("    %s\n", ext.constData());

    info += QString::asprintf("Color format: %u\nDepth-stencil format: %u\n",
                              mWindow->colorFormat(), mWindow->depthStencilFormat());

    info += QStringLiteral("Supported sample counts:");
    const QList<int> sampleCounts = mWindow->supportedSampleCounts();
    for (int count : sampleCounts)
        info += QLatin1Char(' ') + QString::number(count);
    info += QLatin1Char('\n');

    qDebug(info.toUtf8().constData());
    qDebug("\n ***************************** Vulkan Hardware Info finished ******************************************* \n");
}

void RenderWindow::releaseSwapChainResources()
{
    qDebug("\n ***************************** releaseSwapChainResources ******************************************* \n");
}

void RenderWindow::releaseResources()
{
    qDebug("\n ***************************** releaseResources ******************************************* \n");

    VkDevice dev = mWindow->device();

    if (mPipeline) {
        mDeviceFunctions->vkDestroyPipeline(dev, mPipeline, nullptr);
        mPipeline = VK_NULL_HANDLE;
    }

    if (mPipelineLayout) {
        mDeviceFunctions->vkDestroyPipelineLayout(dev, mPipelineLayout, nullptr);
        mPipelineLayout = VK_NULL_HANDLE;
    }

    if (mPipelineCache) {
        mDeviceFunctions->vkDestroyPipelineCache(dev, mPipelineCache, nullptr);
        mPipelineCache = VK_NULL_HANDLE;
    }

    if (mDescriptorSetLayout) {
        mDeviceFunctions->vkDestroyDescriptorSetLayout(dev, mDescriptorSetLayout, nullptr);
        mDescriptorSetLayout = VK_NULL_HANDLE;
    }

    if (mDescriptorPool) {
        mDeviceFunctions->vkDestroyDescriptorPool(dev, mDescriptorPool, nullptr);
        mDescriptorPool = VK_NULL_HANDLE;
    }

    if (mBuffer) {
        mDeviceFunctions->vkDestroyBuffer(dev, mBuffer, nullptr);
        mBuffer = VK_NULL_HANDLE;
    }

    if (mBufferMemory) {
        mDeviceFunctions->vkFreeMemory(dev, mBufferMemory, nullptr);
        mBufferMemory = VK_NULL_HANDLE;
    }
}


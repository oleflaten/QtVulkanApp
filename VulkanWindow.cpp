#include "VulkanWindow.h"
#include "RenderWindow.h"

VulkanWindow::VulkanWindow()
{
}

QVulkanWindowRenderer *VulkanWindow::createRenderer()
{
    //Makes a new instance of the RenderWindow (our Renderer) class
    return new RenderWindow(this, true); // try MSAA, when available
}

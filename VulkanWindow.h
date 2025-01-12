#ifndef VULKANWINDOW_H
#define VULKANWINDOW_H

#include <QVulkanWindow>

/*The QVulkanWindow subclass reimplements the factory function QVulkanWindow::createRenderer().
This returns a new instance of the QVulkanWindowRenderer subclass.
In order to be able to access various Vulkan resources via the window object,
a pointer to the window is passed and stored via the constructor.*/
class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    VulkanWindow();

    QVulkanWindowRenderer* createRenderer() override;

signals:
    void frameQueued(int colorValue);

};
#endif // VULKANWINDOW_H

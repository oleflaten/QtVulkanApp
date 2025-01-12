#include "VulkanWindow.h"
#include "RenderWindow.h"
#include <QKeyEvent>

VulkanWindow::VulkanWindow()
{  }

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    //Makes a new instance of the RenderWindow (our Renderer) class
    return new RenderWindow(this, true); // last true == try MSAA
}

void VulkanWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A)
    {
        qDebug("I pressed the A button");
    }
    if(event->key() == Qt::Key_S)
    {
        qDebug("I pressed the S button");
    }
    if (event->key() == Qt::Key_Escape)
    {
        QCoreApplication::quit();       //Shuts down the whole program
    }
}

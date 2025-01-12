#ifndef HELLOVULKANWIDGET_H
#define HELLOVULKANWIDGET_H

#include "RenderWindow.h "
#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QTabWidget)
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit)

//Forward declaration
class VulkanWindow;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(VulkanWindow *w, QPlainTextEdit *logWidget);

public slots:

    void onGrabRequested();

private:
    VulkanWindow *mWindow{ nullptr };
    QTabWidget *mInfoTab{ nullptr };
    QPlainTextEdit *mInfo{ nullptr };
    QPlainTextEdit *mLogWidget{ nullptr };
};


//
class VulkanRenderer : public RenderWindow
{
public:
    VulkanRenderer(VulkanWindow *w);

    void initResources() override;
};


/*The QVulkanWindow subclass reimplements the factory function QVulkanWindow::createRenderer().
This returns a new instance of the QVulkanWindowRenderer subclass.
In order to be able to access various Vulkan resources via the window object,
a pointer to the window is passed and stored via the constructor.*/
class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    QVulkanWindowRenderer *createRenderer() override;

signals:
    void frameQueued(int colorValue);
};

#endif // HELLOVULKANWIDGET_H

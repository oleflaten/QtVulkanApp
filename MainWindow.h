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
    QPlainTextEdit *mLogWidget{ nullptr };
};

//
class VulkanRenderer : public RenderWindow
{
public:
    VulkanRenderer(VulkanWindow *w);

    void initResources() override;
};

#endif // HELLOVULKANWIDGET_H

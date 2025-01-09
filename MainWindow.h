#ifndef HELLOVULKANWIDGET_H
#define HELLOVULKANWIDGET_H

#include "RenderWindow.h "
#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QTabWidget)
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit)

class VulkanWindow;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(VulkanWindow *w, QPlainTextEdit *logWidget);

public slots:
    void onVulkanInfoReceived(const QString &text);
    void onGrabRequested();

private:
    VulkanWindow *m_window;
    QTabWidget *m_infoTab;
    QPlainTextEdit *m_info;
};

class VulkanRenderer : public RenderWindow
{
public:
    VulkanRenderer(VulkanWindow *w);

    void initResources() override;
    void startNextFrame() override;
};

class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    QVulkanWindowRenderer *createRenderer() override;

signals:
    void vulkanInfoReceived(const QString &text);
    void frameQueued(int colorValue);
};

#endif // HELLOVULKANWIDGET_H

#include <QApplication>
#include <QPlainTextEdit>
#include <QVulkanInstance>
#include <QLibraryInfo>
#include <QLoggingCategory>
#include <QPointer>
#include "MainWindow.h"
#include "VulkanWindow.h"

Q_LOGGING_CATEGORY(lcVk, "qt.vulkan")

static QPointer<QPlainTextEdit> messageLogWidget;
static QtMessageHandler oldMessageHandler{ nullptr };

//Logger system from Qt. Nice to print out messages directly to our program
static void messageHandler(QtMsgType msgType, const QMessageLogContext &logContext, const QString &text)
{
    if (!messageLogWidget.isNull())
        messageLogWidget->appendPlainText(text);
    if (oldMessageHandler)
        oldMessageHandler(msgType, logContext, text);
}

int main(int argc, char *argv[])
{
    //Makes a Qt application
    QApplication app(argc, argv);

    //Logger setup
    messageLogWidget = new QPlainTextEdit(QLatin1String(QLibraryInfo::build()) + QLatin1Char('\n'));
    messageLogWidget->setReadOnly(true);
    oldMessageHandler = qInstallMessageHandler(messageHandler);
    QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    //Qt wrapper for the actual Vulkan Instance
    QVulkanInstance inst;
    inst.setLayers({ "VK_LAYER_KHRONOS_validation" });

    if (!inst.create())
        qFatal("Failed to create Vulkan instance: %d", inst.errorCode());

    //VulkanWindow is the Qt window for our Vulkan Renderer
    VulkanWindow *vulkanWindow = new VulkanWindow;
    //It needs the Vulkan instance
    vulkanWindow->setVulkanInstance(&inst);

    //Main window of our program, that takes our VulkanWindow and logger as input
    MainWindow mainWindow(vulkanWindow, messageLogWidget.data());

    //Sets the size of the program
    mainWindow.resize(1024, 1024);
    //Tells the system to show this main window
    mainWindow.show();

    //app.exec() runs the rest of the program
    return app.exec();
}

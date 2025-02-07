#ifndef HELLOVULKANWIDGET_H
#define HELLOVULKANWIDGET_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QTabWidget)
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit)

//Forward declaration
class VulkanWindow;
class QMenu;
class QMenuBar;
class QAction;
class QDialogButtonBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(VulkanWindow *vw, QPlainTextEdit *logWidget);

public slots:
    void onScreenGrabRequested();

private:
    VulkanWindow *mVulkanWindow{ nullptr };
    QTabWidget *mInfoTab{ nullptr };
    QPlainTextEdit *mLogWidget{ nullptr };

    QMenuBar* createMenu();

    QMenuBar* menuBar{ nullptr };
    QMenu* fileMenu{ nullptr };
    QAction* openFileAction{ nullptr };
    QAction* exitAction{ nullptr };
    std::string mSelectedName;

private slots:
    void openFile();
    void selectName();
};

#endif // HELLOVULKANWIDGET_H

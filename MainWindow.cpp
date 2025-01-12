#include "MainWindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabWidget>
#include "VulkanWindow.h"

MainWindow::MainWindow(VulkanWindow *vw, QPlainTextEdit *logWidget)
    : mVulkanWindow(vw)
{
    //Wraps the VulkanWindow inside a QWidget so we can place it together with other widgets
    QWidget *vulkanWindowWrapper = QWidget::createWindowContainer(vw);

    mLogWidget = logWidget;
    //Sets the colors in the log window
    mLogWidget->setStyleSheet("color: white ; background-color: #2f2f2f ;");

    //makes buttons
    QPushButton *grabButton = new QPushButton(tr("&Grab frame"));
    grabButton->setFocusPolicy(Qt::NoFocus);

    QPushButton *quitButton = new QPushButton(tr("&Quit"));
    quitButton->setFocusPolicy(Qt::NoFocus);

    //connect push of grab button to screen grab function
    connect(grabButton, &QPushButton::clicked, this, &MainWindow::onScreenGrabRequested);
    //connect quit button to quit-function
    connect(quitButton, &QPushButton::clicked, qApp, &QCoreApplication::quit);
    //connect changes in our logger to trigger scroll to end of log window, using a lambda
    connect(mLogWidget, &QPlainTextEdit::textChanged, [logWidget]()
            { logWidget->moveCursor(QTextCursor::End); });

    //Makes the layout of the program, adding items we have made
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(vulkanWindowWrapper, 7);
    mInfoTab = new QTabWidget(this);
    mInfoTab->addTab(mLogWidget, tr("Debug Log"));
    layout->addWidget(mInfoTab, 2);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(grabButton, 1);
    buttonLayout->addWidget(quitButton, 1);
    layout->addLayout(buttonLayout);

    setLayout(layout);

    //sets the keyboard input focus to the RenderWindow when program starts
    //(wrapped inside of this QWidget)
    // - can be deleted, but then you have to click inside the RenderWindow to get the focus
    vulkanWindowWrapper->setFocus();
}

//Makes the screen grab, and saves it to file
void MainWindow::onScreenGrabRequested()
{
    if (!mVulkanWindow->supportsGrab()) {
        QMessageBox::warning(this, tr("Cannot grab"), tr("This swapchain does not support readbacks."));
        return;
    }

    QImage img = mVulkanWindow->grab();

    // Our startNextFrame() implementation is synchronous so img is ready to be
    // used right here.

    QFileDialog fd(this);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setDefaultSuffix("png");
    fd.selectFile("test.png");
    if (fd.exec() == QDialog::Accepted)
        img.save(fd.selectedFiles().first());
}

#include "MainWindow.h"
#include <QVulkanFunctions>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabWidget>
#include "VulkanWindow.h"

MainWindow::MainWindow(VulkanWindow *w, QPlainTextEdit *logWidget)
    : mWindow(w)
{
    QWidget *wrapper = QWidget::createWindowContainer(w);
    mLogWidget = logWidget;
    mLogWidget->setStyleSheet("color: white ; background-color: #2f2f2f ;");


    QPushButton *grabButton = new QPushButton(tr("&Grab frame"));
    grabButton->setFocusPolicy(Qt::NoFocus);

    connect(grabButton, &QPushButton::clicked, this, &MainWindow::onScreenGrabRequested);
    connect(logWidget, &QPlainTextEdit::textChanged, [logWidget]()
        { logWidget->moveCursor(QTextCursor::End); });

    QPushButton *quitButton = new QPushButton(tr("&Quit"));
    quitButton->setFocusPolicy(Qt::NoFocus);

    connect(quitButton, &QPushButton::clicked, qApp, &QCoreApplication::quit);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(wrapper, 7);
    mInfoTab = new QTabWidget(this);
    mInfoTab->addTab(mLogWidget, tr("Debug Log"));
    layout->addWidget(mInfoTab, 2);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(grabButton, 1);
    buttonLayout->addWidget(quitButton, 1);
    layout->addLayout(buttonLayout);

    setLayout(layout);
}

void MainWindow::onScreenGrabRequested()
{
    if (!mWindow->supportsGrab()) {
        QMessageBox::warning(this, tr("Cannot grab"), tr("This swapchain does not support readbacks."));
        return;
    }

    QImage img = mWindow->grab();

    // Our startNextFrame() implementation is synchronous so img is ready to be
    // used right here.

    QFileDialog fd(this);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setDefaultSuffix("png");
    fd.selectFile("test.png");
    if (fd.exec() == QDialog::Accepted)
        img.save(fd.selectedFiles().first());
}

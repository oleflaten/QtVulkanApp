#include "MainWindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabWidget>
#include <QMenuBar>
#include <QLineEdit>
#include <QInputDialog>
#include "VulkanWindow.h"
#include "RenderWindow.h"
#include "VkTriangleSurface.h"

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

    QPushButton *nameButton = new QPushButton(tr("&Name")); // Dag 040225
    nameButton->setFocusPolicy(Qt::NoFocus);                // Dag 040225

    //connect push of grab button to screen grab function
    connect(grabButton, &QPushButton::clicked, this, &MainWindow::onScreenGrabRequested);
    //connect quit button to quit-function
    connect(quitButton, &QPushButton::clicked, qApp, &QCoreApplication::quit);
    //connect changes in our logger to trigger scroll to end of log window, using a lambda
    connect(mLogWidget, &QPlainTextEdit::textChanged, [logWidget]()
            { logWidget->moveCursor(QTextCursor::End); });
    //select file to import
    connect(nameButton, SIGNAL(clicked()), this, SLOT(selectName()));   // Dag 040225

    //Makes the layout of the program, adding items we have made
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMenuBar(createMenu());
    layout->addWidget(vulkanWindowWrapper, 7);
    mInfoTab = new QTabWidget(this);
    mInfoTab->addTab(mLogWidget, tr("Debug Log"));
    layout->addWidget(mInfoTab, 2);
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    buttonLayout->addWidget(nameButton, 1); // Dag 040225
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

QMenuBar *MainWindow::createMenu()
{
    menuBar = new QMenuBar(this);
    fileMenu = new QMenu(tr("&File"), this);
    openFileAction = fileMenu->addAction(tr("&Open file..."));
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);
    menuBar->setVisible(true);
    //
    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    //   editMenu = new QMenu(this);
    //   editNameAction = editMenu->addAction(tr("&Enter name..."));
    //   menuBar->addMenu(editMenu);
    //   editMenu->setVisible(true);
    return menuBar;
}
void MainWindow::openFile() // slot
{
    auto filnavn = QFileDialog::getOpenFileName(this);
    if (!filnavn.isEmpty())
    {
        //QString tekst;
        //loadFile(filnavn, tekst);
        //textEdit->setPlainText(tekst);
        VkTriangleSurface* surf = new VkTriangleSurface(filnavn.toStdString());
        auto rw = dynamic_cast<RenderWindow*>(mVulkanWindow->getRenderWindow());
        rw->getObjects().push_back(surf);
        rw->releaseResources();
        rw->initResources();
    }
}

void MainWindow::selectName()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Object name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
        mSelectedName = text.toStdString();

    auto rw = dynamic_cast<RenderWindow*>(mVulkanWindow->getRenderWindow());
    auto map = rw->getMap();
    auto visualObject = map[mSelectedName];
    if (visualObject != nullptr)
        mVulkanWindow->setSelectedObject(visualObject);
    else {
        QMessageBox msgBox;
        msgBox.setText("Finner ikke " + QString(mSelectedName.c_str()));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setDefaultButton(QMessageBox::Close);
        // int ret = msgBox.exec();
    }
}

#include <QtGui>

#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>

#include "mainform.h"
#include "optionsdialog.h"
#include "graphtools/palette.h"
#include "graphtools/graphtoolsclass.h"
#include "network/connections.h"
#include "network/serverclass.h"
#include "network/clientclass.h"

MainForm::MainForm()
{
    setWindowTitle("TPaint [" + versionString() + "]");
    setWindowIcon(QIcon(":/tpaint.png"));

    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    logDialog       = new LogDialog(this);
    optionsDialog   = new OptionsDialog(this);
    graphWidget     = new GraphWidget(optionsDialog, this);
    paletteWidget   = new Palette(this);
    graphToolsWidget = new GraphToolsClass(this);
    connectionDialog = new ConnectionDialog(this, graphWidget, logDialog, optionsDialog, this);

    connect(paletteWidget, SIGNAL(foregroundColorChanged(QColor)), graphWidget, SLOT(setForegroundColor(QColor)));
    connect(paletteWidget, SIGNAL(backgroundColorChanged(QColor)), graphWidget, SLOT(setBackgroundColor(QColor)));
    connect(graphToolsWidget, SIGNAL(toolChanged(GraphWidget::ToolType)), graphWidget, SLOT(setTool(GraphWidget::ToolType)));
    connect(graphToolsWidget, SIGNAL(thicknessPenChanged(int)), graphWidget, SLOT(setThicknessPen(int)));
    connect(graphToolsWidget, SIGNAL(thicknessEraserChanged(int)), graphWidget, SLOT(setThicknessEraser(int)));
    connect(graphToolsWidget, SIGNAL(shotSignal()), graphWidget, SLOT(makeScreenShot()));

    graphToolsWidget->initGraphToolsClass();

    sendImageButton = new QPushButton(tr("Send Image"));
    connect(sendImageButton, SIGNAL(clicked()), graphWidget, SLOT(sendWholeImage()));

    this->createActions();
    this->createMenus();

    // Main Window
    QHBoxLayout *paletteLayout = new QHBoxLayout;
    paletteLayout->addWidget(paletteWidget);
    paletteLayout->addStretch();
    paletteLayout->addWidget(sendImageButton);
    paletteLayout->setContentsMargins(4, 0, 8, 0);
    paletteLayout->setSpacing(0);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(graphToolsWidget);
    mainLayout->addWidget(graphWidget);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(mainLayout);
    leftLayout->addLayout(paletteLayout);
    leftLayout->setSpacing(0);
    leftLayout->setMargin(0);

    widget->setLayout(leftLayout);

    statusConnectionLabel = new QLabel(tr("Not Connected"));
    statusNickLabel = new QLabel;
    statusNetworkReceivedLabel = new QLabel(tr("Received: %1").arg("0" + tr(" KB")));
    statusNetworkReceivedLabel->setAlignment(Qt::AlignCenter);
    statusNetworkSentLabel = new QLabel(tr("Sent: %1").arg("0" + tr(" KB")));
    statusNetworkSentLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addWidget(statusConnectionLabel, 1);
    statusBar()->addWidget(statusNickLabel, 1);
    statusBar()->addWidget(statusNetworkReceivedLabel);
    statusBar()->addWidget(statusNetworkSentLabel);
}

void MainForm::createActions()
{
//	File
        saveImageAct = new QAction(tr("&Save Image"), this);
        saveImageAct->setShortcut(QString("Ctrl+S"));
        connect(saveImageAct, SIGNAL(triggered()), this, SLOT(saveImage()));

        loadImageAct = new QAction(tr("L&oad Image"), this);
        loadImageAct->setShortcut(QString("Ctrl+O"));
        connect(loadImageAct, SIGNAL(triggered()), this, SLOT(loadImage()));

        optionsAct = new QAction(tr("O&ptions"), this);
        optionsAct->setShortcut(QString("Ctrl+P"));
        connect(optionsAct, SIGNAL(triggered()), optionsDialog, SLOT(show()));

        exitAct = new QAction(tr("&Quit"), this);
        exitAct->setShortcut(QString("Ctrl+Q"));
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

//	Connection
        connectionAct = new QAction(tr("Connect..."), this);
        connect(connectionAct, SIGNAL(triggered()), connectionDialog, SLOT(show()));

        logViewerAct = new QAction(tr("Log viewer"), this);
        logViewerAct->setShortcut(QString("Ctrl+D"));
        connect(logViewerAct, SIGNAL(triggered()), logDialog, SLOT(show()));

//      Image
        clearImageAct = new QAction(tr("&Clear Board"), this);
        clearImageAct->setShortcut(QString("Ctrl+L"));
        connect(clearImageAct, SIGNAL(triggered()), graphWidget, SLOT(clearImage()));

        makeScreenShotAct = new QAction(tr("Make &Screenshot"), this);
        makeScreenShotAct->setShortcut(QString("Ctrl+C"));
        connect(makeScreenShotAct, SIGNAL(triggered()), graphWidget, SLOT(makeScreenShot()));

//	About
        aboutAct = new QAction(tr("&About TPaint..."), this);
        connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

        aboutQtAct = new QAction(tr("About &Qt"), this);
        connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainForm::createMenus()
{
        fileMenu = menuBar()->addMenu(tr("&File"));
        fileMenu->addAction(saveImageAct);
        fileMenu->addAction(loadImageAct);
        fileMenu->addSeparator();
        fileMenu->addAction(optionsAct);
        fileMenu->addSeparator();
        fileMenu->addAction(exitAct);

        connectionMenu = menuBar()->addMenu(tr("Connection"));
        connectionMenu->addAction(connectionAct);
        connectionMenu->addSeparator();
        connectionMenu->addAction(logViewerAct);

        imageMenu = menuBar()->addMenu(tr("Image"));
        imageMenu->addAction(clearImageAct);
        imageMenu->addAction(makeScreenShotAct);

        helpMenu = menuBar()->addMenu(tr("&Help"));
        helpMenu->addAction(aboutAct);
        helpMenu->addAction(aboutQtAct);
}

void MainForm::loadImage() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"));
    if (!filename.isEmpty()) graphWidget->loadFromFile(filename);
}

void MainForm::saveImage() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::homePath() + QDir::separator() + "untitled.png", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!filename.isEmpty()) graphWidget->saveToFile(filename);
}

void MainForm::about()
{
    QString html =
      tr("<html>"
         "<h2>Tpaint [%1]</h2>"
         "<p>"
         "Network core: ncore [%2]<br>Revision: %3<br><br>"
         "TPaint is the simple program for painting together through internet<br>"
         "See website: <a href=http://code.google.com/p/tpaint>"
         "http://code.google.com/p/tpaint</a><br>"
         "<p align=right>evatux, 2011</p>"
         "<p align=justify><small>"
         "You can redistribute or modify it under the terms of the "
         "GNU Lesser General Public License as published "
         "by the Free Software Foundation. "
         "This program is distributed <i>without any warranty</i>. "
         "See the GNU Lesser General Public License for more details."
         "</small></p>"
         "</html>")
      .arg(versionString()).arg(connectionsClass::versionString()).arg(revisonString());

    QMessageBox::about(this, tr("About TPaint"), html);
}

void MainForm::closeEvent(QCloseEvent *event) {
    connectionDialog->stopConnection();
    if (optionsDialog) optionsDialog->saveSettings();
    event->accept();
}

void MainForm::statusUpdate(const StatusType &statusType, const QString &status) {
    switch (statusType) {
        case Connection:        statusConnectionLabel->setText(status);                                 break;
        case Nick:              statusNickLabel->setText(status);                                       break;
        case NetworkReceived:   statusNetworkReceivedLabel->setText(tr("Received: %1").arg(status));    break;
        case NetworkSent:       statusNetworkSentLabel->setText(tr("Sent: %1").arg(status));            break;
    }
}

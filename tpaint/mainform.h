#ifndef MAINFORM_H
#define MAINFORM_H

#include <QMainWindow>
#include "graphwidget.h"
#include "logdialog.h"
#include "connectiondialog.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE

class connectionsClass;
class Palette;
class GraphToolsClass;
class OptionsDialog;

class MainForm : public QMainWindow
{
	Q_OBJECT
public:
    enum StatusType {Connection, Nick, NetworkSent, NetworkReceived};

    MainForm();

    static QString versionString() { return QString("0.2"); }
    static QString revisonString() { return QString("r23"); }

    void statusUpdate(const StatusType& statusType, const QString& status);

private slots:

    void loadImage();
    void saveImage();
    void about();

protected:
    void closeEvent (QCloseEvent * event);

private:
    void createActions();
    void createMenus();

private:
    LogDialog       *logDialog;
    OptionsDialog   *optionsDialog;
    GraphWidget     *graphWidget;
    Palette         *paletteWidget;
    GraphToolsClass *graphToolsWidget;
    ConnectionDialog *connectionDialog;

    QPushButton     *sendImageButton;

    // Menus
    QMenu		*fileMenu;
    QMenu           *connectionMenu;
    QMenu           *imageMenu;
    QMenu		*helpMenu;

    // File Menu
    QAction		*saveImageAct;
    QAction		*loadImageAct;
    QAction         *optionsAct;
    QAction		*exitAct;

    // Tools Menu
    QAction         *connectionAct;
    QAction         *logViewerAct;

    // Image Menu
    QAction         *clearImageAct;
    QAction         *makeScreenShotAct;

    // Help Menu
    QAction		*aboutAct;
    QAction		*aboutQtAct;

    // Status bar
    QLabel          *statusConnectionLabel;
    QLabel          *statusNickLabel;
    QLabel          *statusNetworkSentLabel;
    QLabel          *statusNetworkReceivedLabel;
};

#endif

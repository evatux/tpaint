#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE
class connectionsClass;
class GraphWidget;
class LogDialog;
class OptionsDialog;
class MainForm;

class ConnectionDialog : public QDialog
{
    Q_OBJECT
public:
    ConnectionDialog(MainForm* mForm, GraphWidget* gWidget, LogDialog* lDialog, OptionsDialog* oDialog, QWidget *parent = 0);

public slots:
    void stopConnection();

private slots:
    void startServer();
    void startClient();

    void updateButtons();

    void deleteConnection();

    void sendGreeting();

    void statusConnectionEstablished();
    void statusConnectionClosed();
    void statusConnectionInterrupted();
    void statusNick(const QString&);
    void statusNetworkSent(const int&);
    void statusNetworkReceived(const int&);

private:
    MainForm        *mainForm;
    GraphWidget     *graphWidget;
    LogDialog       *logDialog;
    OptionsDialog   *optionDialog;

    bool            isServer;
    bool            isConnected;
    connectionsClass *connection;

    // Connection Items
    QLabel          *ipLabel;
    QLineEdit       *ipEdit;
    QLabel          *portLabel;
    QLineEdit       *portEdit;

    QLabel          *ipServerLabel;
    QLabel          *portServerLabel;
    QLineEdit       *portServerEdit;

    QTextEdit       *ipsListTextEdit;

    QPushButton     *connectButton;
    QPushButton     *disconnectButton;
    QPushButton     *startServerButton;
    QPushButton     *stopServerButton;
    QPushButton     *closeButton;
};

#endif // CONNECTIONDIALOG_H

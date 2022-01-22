#include "mainform.h"
#include "connectiondialog.h"
#include "graphwidget.h"
#include "logdialog.h"
#include "optionsdialog.h"
#include "network/connections.h"
#include "network/clientclass.h"
#include "network/serverclass.h"

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLayout>
#include <QGroupBox>
#include <QMessageBox>

ConnectionDialog::ConnectionDialog(MainForm *mForm, GraphWidget *gWidget, LogDialog *lDialog, OptionsDialog* oDialog, QWidget *parent) :
    QDialog(parent), mainForm(mForm),
    graphWidget(gWidget), logDialog(lDialog),
    optionDialog(oDialog),
    isServer(0), isConnected(0), connection(0)
{
    setWindowTitle(tr("Connection Wizard"));
    setWindowFlags(Qt::Dialog);

    QGroupBox *clientGroupBox = new QGroupBox(tr("Connect to server"));
    ipLabel		= new QLabel(tr("Ip: "));
    ipEdit		= new QLineEdit("192.168.1.10");
    portLabel           = new QLabel(tr("Port: "));
    portEdit            = new QLineEdit("4123");
    ipLabel->setBuddy(ipEdit);
    portLabel->setBuddy(portEdit);
    connectButton	= new QPushButton(tr("Connect"));
    disconnectButton = new QPushButton(tr("Disconnect"));

    QGroupBox *serverGroupBox = new QGroupBox(tr("Create server"));
    ipServerLabel	= new QLabel(tr("Ip: ") + connectionsClass::getIp());
    portServerLabel     = new QLabel(tr("Port: "));
    portServerEdit	= new QLineEdit("4123");
    portServerLabel->setBuddy(portServerEdit);
    startServerButton   = new QPushButton(tr("Start"));
    stopServerButton    = new QPushButton(tr("Stop"));

    ipsListTextEdit     = new QTextEdit();
    ipsListTextEdit->setReadOnly(true);
    QStringList list = connectionsClass::getIps();
    QString str;
    foreach(str, list)
        ipsListTextEdit->append(str);

    closeButton	= new QPushButton(tr("Close"));

    connect(connectButton, SIGNAL(clicked()), this, SLOT(startClient()));
    connect(disconnectButton, SIGNAL(clicked()), this, SLOT(stopConnection()));
    connect(startServerButton, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(stopServerButton, SIGNAL(clicked()), this, SLOT(stopConnection()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // Client Box
    QHBoxLayout *ipLayout = new QHBoxLayout;
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(ipEdit);

    QHBoxLayout *portLayout = new QHBoxLayout;
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portEdit);

    QHBoxLayout *buttonsClientLayout = new QHBoxLayout;
    buttonsClientLayout->addWidget(connectButton);
    buttonsClientLayout->addWidget(disconnectButton);

    QVBoxLayout *clientLayout = new QVBoxLayout;
    clientLayout->addLayout(ipLayout);
    clientLayout->addLayout(portLayout);
    clientLayout->addLayout(buttonsClientLayout);

    clientGroupBox->setLayout(clientLayout);

    // Server Box
    QHBoxLayout *portServerLayout = new QHBoxLayout;
    portServerLayout->addWidget(portServerLabel);
    portServerLayout->addWidget(portServerEdit);

    QHBoxLayout *buttonsServerLayout = new QHBoxLayout;
    buttonsServerLayout->addWidget(startServerButton);
    buttonsServerLayout->addWidget(stopServerButton);

    QVBoxLayout *serverLayout = new QVBoxLayout;
    serverLayout->addWidget(ipsListTextEdit);
//    serverLayout->addWidget(ipServerLabel);
    serverLayout->addLayout(portServerLayout);
    serverLayout->addLayout(buttonsServerLayout);

    serverGroupBox->setLayout(serverLayout);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(clientGroupBox);
    mainLayout->addWidget(serverGroupBox);
    mainLayout->addLayout(bottomLayout);

    updateButtons();
}

void ConnectionDialog::updateButtons() {
    if (!connection) {
        ipEdit->setEnabled(true);
        portEdit->setEnabled(true);
        connectButton->setEnabled(true);
        disconnectButton->setEnabled(false);
        portServerEdit->setEnabled(true);
        startServerButton->setEnabled(true);
        stopServerButton->setEnabled(false);
    } else {
        if (connection->getConnectionType() == connectionsClass::Server) {
            ipEdit->setEnabled(false);
            portEdit->setEnabled(false);
            connectButton->setEnabled(false);

            portServerEdit->setEnabled(false);
            startServerButton->setEnabled(false);
            stopServerButton->setEnabled(true);
        } else {
            portServerEdit->setEnabled(false);
            startServerButton->setEnabled(false);

            ipEdit->setEnabled(false);
            portEdit->setEnabled(false);
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(true);
        }
    }
}

void ConnectionDialog::stopConnection() {
    if (connection) connection->stop();
    this->deleteConnection();
}

void ConnectionDialog::deleteConnection() {
    disconnect(connection);
    disconnect(graphWidget, 0, connection, 0);
    connection = NULL;

    isConnected = false;
    isServer = false;
    updateButtons();
}

void ConnectionDialog::sendGreeting() {
    connection->sendGreeting(optionDialog->getNickName());
}

void ConnectionDialog::startServer() {
    if (connection) {
        qDebug() << "Smth strange! 'Start Server' called, but connection != null";
        logDialog->debugMessageReceived(tr("Smth strange! 'Start Server' called, but connection != null"));

        this->deleteConnection();
    }
    connection = new serverClass();

    mainForm->statusUpdate(MainForm::Connection, tr("Waiting for connection..."));

    connect(connection, SIGNAL(connectionEstablished(QString)), this, SLOT(statusConnectionEstablished()));
    connect(connection, SIGNAL(connectionClosed()), this, SLOT(statusConnectionClosed()));
    connect(connection, SIGNAL(connectionInterrupted()), this, SLOT(statusConnectionInterrupted()));
    connect(connection, SIGNAL(greetingMessage(QString)), this, SLOT(statusNick(QString)));
    connect(connection, SIGNAL(byteReceivedUpdate(int)), this, SLOT(statusNetworkReceived(int)));
    connect(connection, SIGNAL(byteSentUpdate(int)), this, SLOT(statusNetworkSent(int)));

    connect(connection, SIGNAL(greetingMessage(QString)), this, SLOT(sendGreeting()));
    connect(connection, SIGNAL(messageReceive(QByteArray)), graphWidget, SLOT(imageUpdate(QByteArray)));
    connect(graphWidget, SIGNAL(imageModified(QByteArray)), connection, SLOT(messageSend(QByteArray)));

    connect(connection, SIGNAL(debugMessage(QString)), logDialog, SLOT(debugMessageReceived(QString)));
    if (!connection->start("", portServerEdit->text().toInt())) {
        QMessageBox::critical(0, tr("Server Error"), tr("Server not started"));
        this->deleteConnection();
        statusConnectionClosed();
        return;
    }

    connect(connection, SIGNAL(connectionClosed()), this, SLOT(deleteConnection()));
    connect(connection, SIGNAL(connectionInterrupted()), this, SLOT(deleteConnection()));

    isConnected = true;
    isServer = true;
    updateButtons();
}

void ConnectionDialog::startClient() {
    if (connection) {
        qDebug() << "Smth strange! 'Start Client' called, but connection != null";
        logDialog->debugMessageReceived(tr("Smth strange! 'Start Client' called, but connection != null"));

        this->deleteConnection();
    }
    connection = new clientClass();

    mainForm->statusUpdate(MainForm::Connection, tr("Connecting..."));

    connect(connection, SIGNAL(connectionEstablished(QString)), this, SLOT(statusConnectionEstablished()));
    connect(connection, SIGNAL(connectionClosed()), this, SLOT(statusConnectionClosed()));
    connect(connection, SIGNAL(connectionInterrupted()), this, SLOT(statusConnectionInterrupted()));
    connect(connection, SIGNAL(greetingMessage(QString)), this, SLOT(statusNick(QString)));
    connect(connection, SIGNAL(byteReceivedUpdate(int)), this, SLOT(statusNetworkReceived(int)));
    connect(connection, SIGNAL(byteSentUpdate(int)), this, SLOT(statusNetworkSent(int)));

    connect(connection, SIGNAL(messageReceive(QByteArray)), graphWidget, SLOT(imageUpdate(QByteArray)));
    connect(graphWidget, SIGNAL(imageModified(QByteArray)), connection, SLOT(messageSend(QByteArray)));

    connect(connection, SIGNAL(debugMessage(QString)), logDialog, SLOT(debugMessageReceived(QString)));
    if (!connection->start(ipEdit->text(), portEdit->text().toInt())) {
        QMessageBox::critical(0, tr("Client Error"), tr("Clients not started"));
        this->deleteConnection();
        statusConnectionClosed();
        return;
    }

    connect(connection, SIGNAL(connectionClosed()), this, SLOT(deleteConnection()));
    connect(connection, SIGNAL(connectionInterrupted()), this, SLOT(deleteConnection()));

    isConnected = true;
    isServer = false;
    updateButtons();
    this->sendGreeting();
}

void ConnectionDialog::statusConnectionEstablished() {
    mainForm->statusUpdate(MainForm::Connection, tr("Connection established"));
}

void ConnectionDialog::statusConnectionClosed() {
    mainForm->statusUpdate(MainForm::Connection, tr("Connection closed"));
    mainForm->statusUpdate(MainForm::Nick, "");
    mainForm->statusUpdate(MainForm::NetworkReceived, "0" + tr(" KB"));
    mainForm->statusUpdate(MainForm::NetworkSent, "0" + tr(" KB"));
}

void ConnectionDialog::statusConnectionInterrupted() {
    mainForm->statusUpdate(MainForm::Connection, tr("Connection interrupted"));
    mainForm->statusUpdate(MainForm::Nick, "");
    mainForm->statusUpdate(MainForm::NetworkReceived, "0" + tr(" KB"));
    mainForm->statusUpdate(MainForm::NetworkSent, "0" + tr(" KB"));
}

void ConnectionDialog::statusNick(const QString & nick) {
    if (isServer) mainForm->statusUpdate(MainForm::Nick, tr("Client: %1").arg(nick));
    else mainForm->statusUpdate(MainForm::Nick, tr("Server: %1").arg(nick));
}

void ConnectionDialog::statusNetworkReceived(const int & bytes) {
    mainForm->statusUpdate(MainForm::NetworkReceived, QString::number(bytes/1024) + tr(" KB"));
}

void ConnectionDialog::statusNetworkSent(const int & bytes) {
    mainForm->statusUpdate(MainForm::NetworkSent, QString::number(bytes/1024) + tr(" KB"));
}

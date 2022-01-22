#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

#include <QObject>
#include <QTcpServer>

#include "connections.h"

class QTcpSocket;
class QString;

class serverClass : public connectionsClass
{
    Q_OBJECT

public:
    serverClass(QObject *parent = 0);

    int start(const QString& ipString, int port);
    void stop();
    ConnectionType getConnectionType() { return Server; }
    bool isConnected();

    int startServer(int port);
    void stopServer();
    void sendToClient(const QString& str);

public slots:
    void messageSend(const QByteArray& arr);
    void sendGreeting(const QString &nick);

private slots:
    void slotNewConnection();
    void slotReadClient   ();
    void clientDisconnected();

private:

    void __send__function(const QString& protoStr, const QByteArray& arr);
    void __stop__function(StopType type);

private:
    QTcpServer  tcpServer;
    QTcpSocket  *tcpClientSocket;
    int         port;

    quint32     m_nNextBlockSize;
};

#endif // SERVER_CLASS_H

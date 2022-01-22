#ifndef CLIENT_CLASS_H
#define CLIENT_CLASS_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

#include "connections.h"

class clientClass : public connectionsClass
{
    Q_OBJECT

public:
    clientClass(QObject *parent = 0);

    int start(const QString& ipString, int port);
    void stop();
    ConnectionType getConnectionType() { return Client; }
    bool isConnected();

public slots:
    void messageSend(const QByteArray& arr);
    void sendGreeting(const QString &nick);

private slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotConnected   (                            );
    void serverDisconnected();

private:
    void __send__function(const QString& protoStr, const QByteArray& arr);
    void __stop__function(StopType type);

private:
    QTcpSocket      tcpSocket;
    QHostAddress    serverIp;
    int             port;

    quint32         m_nNextBlockSize;
};

#endif // CLIENT_CLASS_H

#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <QObject>

class QString;

class connectionsClass : public QObject
{
    Q_OBJECT

public:
    enum ConnectionType {Server, Client};
    enum StopType {ClosedByServer, ClosedByClient, Interrupted};

    connectionsClass(QObject *parent = 0);

    virtual int start(const QString& ipString, int port) =0;
    virtual void stop() =0;
    virtual ConnectionType getConnectionType() =0;
    virtual bool isConnected() =0;

    static QString getIp();
    static QStringList getIps();
    static QString versionString() { return QString("0.3"); }

signals:
    void    messageReceive(const QByteArray& data);
    void    connectionEstablished(const QString& str);
    void    connectionClosed();
    void    connectionInterrupted();
    void    errorOccur(const QString& str);
    void    debugMessage(const QString& str);
    void    greetingMessage(const QString& nick);

    void    byteSentUpdate(const int& bytes);
    void    byteReceivedUpdate(const int& bytes);

public slots:
    virtual void messageSend(const QByteArray& data) =0;
    virtual void sendGreeting(const QString& nick) =0;

protected:
    int         byteSent;
    int         byteReceived;

private:
    QString     ipString;
    int         port;

};

#endif // CONNECTIONS_H

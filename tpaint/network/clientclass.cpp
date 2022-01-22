#include "clientclass.h"

#include <QtNetwork>

clientClass::clientClass(QObject *parent) : connectionsClass(parent),
        port(0), m_nNextBlockSize(0)
{
    connect(&tcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(&tcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(&tcpSocket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));
    connect(&tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
}

int clientClass::start(const QString &ipString, int port) {
    qDebug() << "Client: 'start' is called";
    emit debugMessage(tr("Client: 'start' called"));

    serverIp = QHostAddress(ipString);
    this->port = port;

    tcpSocket.connectToHost(serverIp, port);
    if (!tcpSocket.waitForConnected(10000)) {
        qDebug() << "Client: connection wasn't set. timeout.";
        emit debugMessage(tr("Client: connection to %1 : %2 wasn't set. timeout.").arg(ipString).arg(port));
        tcpSocket.abort();
        emit errorOccur(tr("Client: connection to %1 : %2 wasn't set. timeout.").arg(ipString).arg(port));
        return 0;
    }

    return 1;
}

void clientClass::stop() {
    this->__stop__function(ClosedByClient);
}

void clientClass::__stop__function(StopType type) {
    tcpSocket.disconnect();
    if (type == ClosedByClient) {
        if (tcpSocket.state() == QAbstractSocket::ConnectedState) this->__send__function("D", QByteArray());

        qDebug() << "Client: 'stop' is called";
        emit debugMessage(tr("Client: 'stop' is called"));
        emit connectionClosed();
    } else if (type == ClosedByServer) {
        qDebug() << "Client: server closed the connection";
        emit debugMessage(tr("Client: server closed the connection"));
        emit connectionClosed();
    } else if (type == Interrupted) {
        qDebug() << "Client: the connection was interrupted";
        emit debugMessage(tr("Client: the connection was interrupted"));
        emit connectionInterrupted();
        emit connectionClosed();
    }
    if (tcpSocket.isValid()) tcpSocket.abort();
    tcpSocket.close();
}

bool clientClass::isConnected() {
    return (tcpSocket.state() == QAbstractSocket::ConnectedState);
}

void clientClass::slotReadyRead()
{
    QDataStream in(&tcpSocket);
    in.setVersion(QDataStream::Qt_4_7);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (tcpSocket.bytesAvailable() < sizeof(quint32)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (tcpSocket.bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        byteReceived += (sizeof(quint32) + tcpSocket.bytesAvailable());
        emit byteReceivedUpdate(byteReceived);

        QString protoStr;
        QByteArray arr;
        in >> protoStr >> arr;

        if (protoStr == "D") {
            this->__stop__function(ClosedByServer);
        } else if (protoStr == "M"){
            qDebug() << "Client: message received";
            emit debugMessage(tr("Client: message received"));

            emit messageReceive(arr);
        } else if (protoStr == "G") {
            qDebug() << "Joined to " + QString::fromUtf8(arr);
            emit debugMessage(tr("Client: joined to '%1'").arg(QString::fromUtf8(arr)));
            emit greetingMessage(QString::fromUtf8(arr));
        } else {
            qDebug() << "Client: unexpected protoChar = '" + protoStr + "'";
            emit debugMessage(tr("Client: unexpected protoChar = '%1'").arg(protoStr));
        }

        m_nNextBlockSize = 0;
    }
}

void clientClass::slotError(QAbstractSocket::SocketError err)
{
    QString errorString =
        tr("Error: ") + (err == QAbstractSocket::HostNotFoundError ?
                     tr("The host was not found.") :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     tr("The remote host is closed.") :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     tr("The connection was refused.") :
                     QString(tcpSocket.errorString())
                    );
    emit errorOccur(errorString);
    emit debugMessage(errorString);
}

void clientClass::messageSend(const QByteArray& arr)
{
    this->__send__function("M", arr);
}

void clientClass::sendGreeting(const QString &nick) {
    this->__send__function("G", nick.toUtf8());
}

void clientClass::__send__function(const QString& protoStr, const QByteArray &arr) {
    if (tcpSocket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Client: trying to send message, but state is not connected!";
        emit debugMessage(tr("Client: trying to send message, but state is not connected!"));

        emit errorOccur(tr("Client: trying to send message, but state is not connected!"));
        return;
    }

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << quint32(0) << protoStr << arr;

    out.device()->seek(0);
    out << quint32(arrBlock.size() - sizeof(quint32));

    tcpSocket.write(arrBlock);
    tcpSocket.flush();

    byteSent += arrBlock.size();
    emit byteSentUpdate(byteSent);

    qDebug() << "Client: the message was sent. protoChar = '" + protoStr + "'";
    emit debugMessage(tr("Client: the message was sent. protoChar = '%1'").arg(protoStr));
}

void clientClass::slotConnected()
{
    qDebug() << "Client: connected to " + serverIp.toString() + " : " + QString::number(port) + ", " + QString::number(tcpSocket.peerPort());
    emit debugMessage(tr("Client: connected to %1 : %2, %3").arg(serverIp.toString()).arg(port).arg(tcpSocket.peerPort()));

    emit connectionEstablished(tr("Client: connected to %1 : %2, %3").arg(serverIp.toString()).arg(port).arg(tcpSocket.peerPort()));
}

void clientClass::serverDisconnected() {
    this->__stop__function(Interrupted);
}

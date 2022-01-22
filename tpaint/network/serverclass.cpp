#include "serverclass.h"

#include <QtNetwork>

serverClass::serverClass(QObject *parent) : connectionsClass(parent),
        tcpClientSocket(0), port(0),
        m_nNextBlockSize(0)
{
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    tcpServer.setMaxPendingConnections(1);
}

int serverClass::start(const QString& /*ipString*/, int port) {
    qDebug() << "Server: 'start' is called";
    emit debugMessage(tr("Server: 'start' is called"));
    tcpClientSocket = NULL;

    this->port = port;

    if (!tcpServer.listen(QHostAddress::Any, port)) {
        QString errorString = tr("Server: error. Unable to start the server: %1").arg(tcpServer.errorString());
        qDebug() << errorString;
        emit debugMessage(errorString);

        emit errorOccur(errorString);
        tcpServer.close();
        return 0;
    }

    return 1;
}

void serverClass::stop(){
    this->__stop__function(ClosedByServer);
}

void serverClass::__stop__function(StopType type) {
    if (tcpClientSocket) tcpClientSocket->disconnect();
    if (type == ClosedByServer) {
        if (tcpClientSocket) {
            if (tcpClientSocket->state() == QAbstractSocket::ConnectedState) this->__send__function("D", QByteArray());
            if (tcpClientSocket->state() == QAbstractSocket::ConnectedState) tcpClientSocket->abort();
        }

        qDebug() << "Server: 'stop' is called";
        emit debugMessage(tr("Server: 'stop' is called"));
        emit connectionClosed();
    } else if (type == ClosedByClient){
        qDebug() << "Server: client closed the connection";
        emit debugMessage(tr("Server: client closed the connection"));
        emit connectionClosed();
    } else {
        qDebug() << "Server: the connection was interrupted";
        emit debugMessage(tr("Server: the connection was interrupted"));
        emit connectionInterrupted();
        emit connectionClosed();
    }

    if (tcpClientSocket) {
//        delete tcpClientSocket;
        tcpClientSocket = NULL;
    }

    tcpServer.close();
    port = 0;
}

bool serverClass::isConnected() {
    if (!tcpClientSocket) return false;
    return (tcpClientSocket->state() == QAbstractSocket::ConnectedState);
}

void serverClass::messageSend(const QByteArray& arr)
{
    this->__send__function("M", arr);
}

void serverClass::sendGreeting(const QString &nick) {
    this->__send__function("G", nick.toUtf8());
}

void serverClass::__send__function(const QString& protoStr, const QByteArray &arr) {
    if (!tcpClientSocket) {
        qDebug() << "Server: trying to send message, but there is no clients!";
        emit debugMessage(tr("Server: trying to send message, but there is no clients!"));

        emit errorOccur(tr("Server: trying to send message, but there is no clients!"));
        return;
    }

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << quint32(0) << protoStr << arr;

    out.device()->seek(0);
    out << quint32(arrBlock.size() - sizeof(quint32));

    tcpClientSocket->write(arrBlock);
    tcpClientSocket->flush();

    byteSent += arrBlock.size();
    emit byteSentUpdate(byteSent);

    qDebug() << "Server: the message was sent. protoChar = '" + protoStr + "' checksum: " + QString::number(qChecksum(arr.constData(), arr.length()));
    emit debugMessage(tr("Server: the message was sent. protoChar = '%1'").arg(protoStr));
}

void serverClass::slotNewConnection()
{
    qDebug() << "Server: waiting for new connection";
    emit debugMessage(tr("Server: waiting for new connection"));

    if (tcpClientSocket && tcpClientSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Server: VERY STRANGE... new connection, while the connection alredy exist";
        emit debugMessage(tr("Server: VERY STRANGE... new connection, while the connection alredy exist"));
        return;
    }

    tcpClientSocket = tcpServer.nextPendingConnection();
    connect(tcpClientSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(tcpClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    tcpServer.close();
    qDebug() << "Server: server is closed";
    emit debugMessage(tr("Server: server is closed"));

    QString clientInfoString = tcpClientSocket->peerAddress().toString() + " : " + QString::number(tcpClientSocket->peerPort());

    qDebug() << "Server: new connection from " + clientInfoString;
    emit debugMessage(tr("Server: new connection from %1").arg(clientInfoString));

    emit connectionEstablished(tr("Server: new connection from %1").arg(clientInfoString));
}

void serverClass::slotReadClient()
{
    qDebug() << "Server: receive smth";
    emit debugMessage(tr("Server: receive smth"));
//    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QTcpSocket* pClientSocket = tcpClientSocket;
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_7);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint32)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        byteReceived += (sizeof(quint32) + pClientSocket->bytesAvailable());
        emit byteReceivedUpdate(byteReceived);

        QString protoStr;
        QByteArray arr;
        in >> protoStr >> arr;

//        byteReceived += (sizeof(quint32) + sizeof(QChar)*protoStr.length() + arr.size());
        emit byteReceivedUpdate(byteReceived);

        if (protoStr == "D") {
            this->__stop__function(ClosedByClient);
        } else if (protoStr == "M"){
            qDebug() << "Server: message received";
            emit debugMessage(tr("Server: message received"));

            emit messageReceive(arr);
        } else if (protoStr == "G") {
            qDebug() << QString::fromUtf8(arr) + " joined";
            emit debugMessage(tr("Server: '%1' joined").arg(QString::fromUtf8(arr)));
            emit greetingMessage(QString::fromUtf8(arr));
        } else{
            qDebug() << "Server: unexpected protoChar = '" + protoStr + "'";
            emit debugMessage(tr("Server: unexpected protoChar = '%1'").arg(protoStr));
        }

        m_nNextBlockSize = 0;
    }
}

void serverClass::clientDisconnected() {
    this->__stop__function(Interrupted);
}

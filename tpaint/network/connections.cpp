#include "connections.h"

#include <QNetworkInterface>
#include <QStringList>

connectionsClass::connectionsClass(QObject *parent) : QObject(parent),
        byteSent(0), byteReceived(0), port(0)
{
}

QString connectionsClass::getIp() {
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddress;
}

QStringList connectionsClass::getIps() {
    QStringList ipAddresses;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddresses << ipAddressesList.at(i).toString();
        }
    }
    ipAddresses << tr("");
    ipAddresses << QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddresses;
}

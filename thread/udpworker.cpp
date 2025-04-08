#include "udpworker.h"

UdpWorker::UdpWorker(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
}

UdpWorker::~UdpWorker() {
    delete udpSocket;
    delete threadPool;
}

void UdpWorker::connectToHost(const QString &ip, int remote_port, int local_port) {
    if (udpSocket->state() == QAbstractSocket::BoundState) {
        udpSocket->close();
    }
    if (udpSocket->state() == QAbstractSocket::UnconnectedState) {
        udpSocket->abort();
    }
    if (!udpSocket->bind(QHostAddress::Any, local_port)) {
        emit errorOccurred("Failed to bind to local port");
        return;
    }
    this->target_ip = ip;
    this->remote_port = remote_port;
    this->local_port = local_port;
    qDebug() << "udp start bind";
}

void UdpWorker::disconnectFromHost() {
    udpSocket->disconnectFromHost();
    udpSocket->close();
}

void UdpWorker::sendMessage(const QString &message) {
    if (!message.isEmpty()) {
        QByteArray data = message.toUtf8();
        qint64 bytesSent = udpSocket->writeDatagram(data, QHostAddress(target_ip), remote_port);
        if(bytesSent == -1){
            qDebug() << "error for send data";
        }else{
            qDebug() << "send data" << bytesSent << "Bytes into" << target_ip << ":" << remote_port;
        }
    } else {
        emit errorOccurred("Message is empty");
    }
}

void UdpWorker::handleReadyRead() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        emit messageReceived(datagram);
    }
}

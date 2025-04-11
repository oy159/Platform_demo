#include "udpworker.h"

UdpWorker::UdpWorker(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
    connect(this,&UdpWorker::dataReceived,this,&UdpWorker::handleDataReceived);
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

void UdpWorker::handleTransferFinished() {
    // 处理传输完成的逻辑
    is_framehead = false;
}


void UdpWorker::handleReadyRead() {
    while (udpSocket->hasPendingDatagrams()) {

        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        // if(!is_framehead){
        //     // todo: 处理特殊帧头
        //     if(datagram.size() < 10){
        //         is_framehead = true;
        //         // 判断是数据帧还是固件版本信息回传

        //         return;
        //     }
        // }

        // if(is_framehead)
        // if(is_dataframe)
        emit messageReceived(datagram);

        emit dataReceived(datagram);

    }
}

void UdpWorker::handleDataReceived(const QByteArray &data) {
    // 处理接收到的数据
    convertBufferToU16Array(data, AdcDataArray);
    if(transFinished){
        std::vector<double> dataArray;
        // 处理函数
        for (int i = 0; i < AdcDataArray.size(); ++i) {
            dataArray.push_back((double) AdcDataArray[i]);
        }

        // mCaculateParams->setData(dataArray);
        // mCalculateThread->start();
        // QMetaObject::invokeMethod(mCaculateParams, "calculateParams", Qt::QueuedConnection);

        transFinished = false;
        AdcDataArray.clear();
    }
}

void UdpWorker::convertBufferToU16Array(const QByteArray &buffer, std::vector<uint16_t> &u16Array) {
    int size = buffer.size();
    if (size % 2 != 0 ) {
        size -= 1; // Ignore the last byte if the size is odd
        transFinished = true;
    }else if(u16Array.size()*2 + size == 65532*2){
        transFinished = true;
        // emit is_framehead==false
        // QMetaObject::invokeMethod(mUdpWorker, "handleTransferFinished", Qt::QueuedConnection);
    }
    for (int i = 0; i < size; i += 2) {
        uint16_t value = (uint16_t)buffer[i] << 8 | (uint16_t)buffer[i + 1];
        u16Array.push_back(BIG_LITTLE_SWAP16(value));
    }
}
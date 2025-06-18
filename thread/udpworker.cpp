#include "udpworker.h"

UdpWorker::UdpWorker(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);
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


    // todo: 获取固件版本
    uint32_t command = 0xff7fff7f;
    QByteArray data;
    data.append((char)((command >> 24) & 0xFF));
    data.append((char)((command >> 16) & 0xFF));
    data.append((char)((command >> 8) & 0xFF));
    data.append((char)(command & 0xFF));

    udpSocket->writeDatagram(data, QHostAddress(target_ip), remote_port);
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    timer.start(1000); // 设置超时时间为1秒
    loop.exec();

    if (udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        qDebug() << "get firmware version" << datagram;
    }


    // 再打开数据处理
    disconnect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
}

void UdpWorker::disconnectFromHost() {
    udpSocket->disconnectFromHost();
    udpSocket->close();
    disconnect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
}

void UdpWorker::sendMessage(const QString &message) {
    if (!message.isEmpty()) {
        QByteArray data = message.toUtf8();
        qint64 bytesSent = udpSocket->writeDatagram(data, QHostAddress(target_ip), remote_port);
        if(bytesSent == -1){
            qDebug() << "error for send data";
        }else{
//            qDebug() << "send data" << bytesSent << "Bytes into" << target_ip << ":" << remote_port;
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

        emit dataReceived(datagram);

    }
}

void UdpWorker::handleDataReceived(const QByteArray &data) {
    // 处理接收到的数据
    convertBufferToU16Array(data, AdcDataArray);
    if(transFinished){
        emit ADCDataReady(AdcDataArray);
    }
}

void UdpWorker::convertBufferToU16Array(const QByteArray &buffer, std::vector<uint16_t> &u16Array) {
    int size = buffer.size();
    if (size % 2 != 0 ) {
        size -= 1; // Ignore the last byte if the size is odd
        transFinished = true;
    }else if(u16Array.size()*2 + size == 65532*2){
        transFinished = true;

        is_framehead = false;
    } 
    for (int i = 0; i < size; i += 2) {
        uint16_t value = (((uint16_t)buffer[i] << 8)&0xff00) | ((uint16_t)buffer[i + 1] & 0x00ff);
        u16Array.push_back(BIG_LITTLE_SWAP16(value));
    }
}

void UdpWorker::handleClearADCData(){
    transFinished = false;
    AdcDataArray.clear();
}
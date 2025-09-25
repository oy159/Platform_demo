#include <QtEndian>
#include "udpworker.h"

UdpWorker::UdpWorker(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);
    udpSocket2 = new QUdpSocket(this);

    connect(this,&UdpWorker::dataReceived,this,&UdpWorker::handleDataReceived);
}

UdpWorker::~UdpWorker() {
    delete udpSocket;
    // delete threadPool;
}

void UdpWorker::connectToHost(const QString &ip, int remote_port, int local_port) {
    if (udpSocket->state() == QAbstractSocket::BoundState) {
        udpSocket->close();
    }
    if (udpSocket->state() == QAbstractSocket::UnconnectedState) {
        udpSocket->abort();
    }
    if (!udpSocket->bind(QHostAddress::Any, local_port)) {
        emit errorOccurred("Failed to bind to local port"+ QString::number(local_port));
        return;
    }

    if (udpSocket2->state() == QAbstractSocket::BoundState) {
        udpSocket2->close();
    }
    if (udpSocket2->state() == QAbstractSocket::UnconnectedState) {
        udpSocket2->abort();
    }
    if (!udpSocket2->bind(QHostAddress::Any, 14514)) {
        emit errorOccurred("Failed to bind to local port" + QString::number(14514));
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

        if(datagram.contains("AD9434")){
            emit initializePlatform("AD9434");
            mdeviceType = AD9434;
        }else if (datagram.contains("AD9268")) {
            emit initializePlatform("AD9268");
            mdeviceType = AD9268;
        }else if(datagram.contains("AD9142")){
            emit initializePlatform("AD9142");
            mdeviceType = AD9142;
        }else if(datagram.contains("AD9747")){
            emit initializePlatform("AD9747");
            mdeviceType = AD9747;
        }else if(datagram.contains("AutoDetect")) {
            emit initializePlatform("AutoDetect");
            mdeviceType = AutoDetect;
        }

    }
    // 再打开数据处理
    disconnect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);

    tempTimer = new QTimer(this);
    tempTimer->setInterval(5000); // 5秒间隔
    connect(tempTimer, &QTimer::timeout, this, &UdpWorker::handleGetTemp);
    tempTimer->start();
//    handleGetAutoDetect();
}

void UdpWorker::disconnectFromHost() {
    udpSocket->disconnectFromHost();
    udpSocket->close();
    disconnect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
}

void UdpWorker::sendMessage(const QString &message) {
//    tempTimer->stop();
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
//    tempTimer->start();
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

void UdpWorker::handleSetAD9268Channel(int channel)
{
    if(mdeviceType != AD9268){
        emit  errorOccurred("Now ADC is not AD9268, cannot set channel.");
        return;
    }

    if (channel != 1 && channel != 2) {
        emit errorOccurred("Set AD9268 Invalid channel number. Use 1 or 2.");
        return;
    }

    current_command =  (channel == 1) ? CMD_SET_AD9268_CHANNEL1 : CMD_SET_AD9268_CHANNEL2;
    QByteArray data;
    data.append((char)((current_command >> 24) & 0xFF));
    data.append((char)((current_command >> 16) & 0xFF));
    data.append((char)((current_command >> 8) & 0xFF));
    data.append((char)(current_command & 0xFF));


    disconnect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
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

        // datagram是否为"AD9268 Channel Set Success"的响应
        if(datagram.contains("AD9268 Set Success")){
            qDebug() << "Set AD9268 Channel" << channel << "Success";
        } else {
            emit errorOccurred("Failed to set AD9268 channel");
        }

    }
    disconnect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
}

void UdpWorker::handleSetAD9518ExternalClock(int freq_Hz)
{
    current_command = CMD_ENABLE_AD9915_EXTERNAL_CLOCK;
    QByteArray data;
    data.append((char)((current_command >> 24) & 0xFF));
    data.append((char)((current_command >> 16) & 0xFF));
    data.append((char)((current_command >> 8) & 0xFF));
    data.append((char)(current_command & 0xFF));
    data.append((char)((freq_Hz >> 24) & 0xFF));
    data.append((char)((freq_Hz >> 16) & 0xFF));
    data.append((char)((freq_Hz >> 8) & 0xFF));
    data.append((char)(freq_Hz & 0xFF));


    disconnect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
    udpSocket->writeDatagram(data, QHostAddress(target_ip), remote_port);
    
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    timer.start(5000); // 设置超时时间为1秒
    loop.exec();

    if (udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        qDebug() << "get zynq return: " << datagram;
        // todo: 解析datagram，判断是否设置成功
        if(datagram.contains("Set Success")){
            qDebug() << "External Clock Set success";
        }

    }else{
        qDebug() << "Set Fail";
    }
    disconnect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
}

void UdpWorker::handleSetAD9518InternalClock()
{
    current_command =  CMD_ENABLE_AD9915_INTERNAL_CLOCK;
    QByteArray data;
    data.append((char)((current_command >> 24) & 0xFF));
    data.append((char)((current_command >> 16) & 0xFF));
    data.append((char)((current_command >> 8) & 0xFF));
    data.append((char)(current_command & 0xFF));


    disconnect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
    udpSocket->writeDatagram(data, QHostAddress(target_ip), remote_port);
    
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    timer.start(5000); // 设置超时时间为1秒
    loop.exec();

    if (udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        qDebug() << "get zynq return: " << datagram;
        if(datagram.contains("Set Success")){
            qDebug() << "Internal Clock Set success";
        }

    }
    disconnect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
}

void UdpWorker::handleSetDDSFreq(int freq_Hz, int Mode)
{
    if(mdeviceType == AD9268 || mdeviceType == AD9434){
        emit  errorOccurred("Now is ADC device");
        return;
    }

    QByteArray data;
    if(Mode == 0)
    {
        current_command = CMD_SET_DDS_FREQ;
    } else{
        current_command = CMD_SET_DDS_TWO_TONE;
    }

    uint32_t freq;
    if(mdeviceType == AD9142) {
        freq = (uint32_t) ((double) (qPow(2, 16)) / 2.0e8 * freq_Hz * qPow(2, 16));
    }else if(mdeviceType == AD9747){
        freq = (uint32_t) ((double) (qPow(2, 16)) / 2.5e8 * freq_Hz * qPow(2, 16));
    }
    data.append((char) ((current_command >> 24) & 0xFF));
    data.append((char) ((current_command >> 16) & 0xFF));
    data.append((char) ((current_command >> 8) & 0xFF));
    data.append((char) (current_command & 0xFF));
    data.append((char) ((freq >> 24) & 0xFF));
    data.append((char) ((freq >> 16) & 0xFF));
    data.append((char) ((freq >> 8) & 0xFF));
    data.append((char) (freq & 0xFF));


    uint32_t freq2;
    if(Mode != 0){
        if(mdeviceType == AD9747){
            freq2 = (uint32_t) ((double) (qPow(2, 16)) / 2.5e8 * (freq_Hz + 1.0e6) * qPow(2, 16));
        }else
            freq2 = (uint32_t)((double)(qPow(2,16)) / 2.0e8 * (freq_Hz + 1.0e6) * qPow(2,16));
        data.append((char)((freq2 >> 24) & 0xFF));
        data.append((char)((freq2 >> 16) & 0xFF));
        data.append((char)((freq2 >> 8) & 0xFF));
        data.append((char)(freq2 & 0xFF));
    }

    disconnect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
    udpSocket->writeDatagram(data, QHostAddress(target_ip), remote_port);
    
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    timer.start(5000); // 设置超时时间为1秒
    loop.exec();

    if (udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        qDebug() << "get zynq return: " << datagram;
        if(datagram.contains("Set Success")){
            qDebug() << "DDS Freq Set success";
        }

    }
    disconnect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);

}

void UdpWorker::handleSetDACValue(int value)
{
    if(mdeviceType == AD9268 || mdeviceType == AD9434){
        emit  errorOccurred("Now is ADC device");
        return;
    }

    if(value < 0 || value > 65535){
//        emit errorOccurred("the DAC is 16bit，");
        return;
    }

    current_command = CMD_SET_DAC_VALUE;
    QByteArray data;
    data.append((char)((current_command >> 24) & 0xFF));
    data.append((char)((current_command >> 16) & 0xFF));
    data.append((char)((current_command >> 8) & 0xFF));
    data.append((char)(current_command & 0xFF));
    data.append((char)((value >> 24) & 0xFF));
    data.append((char)((value >> 16) & 0xFF));
    data.append((char)((value >> 8) & 0xFF));
    data.append((char)(value & 0xFF));

    disconnect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
    udpSocket->writeDatagram(data, QHostAddress(target_ip), remote_port);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    timer.start(5000); // 设置超时时间为1秒
    loop.exec();

    if (udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        qDebug() << "get zynq return: " << datagram;
        // todo: 解析datagram，判断是否设置成功
        if(datagram.contains("Set Success")){
            qDebug() << "DAC Value Set success";
        }
        emit DACValueSetSuccess();
    }
    disconnect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);

}

void UdpWorker::handleGetTemp() {
    current_command = CMD_GET_ZYNQ_TEMP;
    QByteArray data;
    data.append((char)((current_command >> 24) & 0xFF));
    data.append((char)((current_command >> 16) & 0xFF));
    data.append((char)((current_command >> 8) & 0xFF));
    data.append((char)(current_command & 0xFF));

    udpSocket2->writeDatagram(data, QHostAddress(target_ip), remote_port);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(udpSocket2, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    timer.start(1000); // 设置超时时间为1秒
    loop.exec();

    if (udpSocket2->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket2->pendingDatagramSize());
        udpSocket2->readDatagram(datagram.data(), datagram.size());

        // todo: 解析datagram，判断是否获取成功
        uint32_t cpuTemp, cpuVolt;
        if (datagram.size() == 2 * sizeof(cpuTemp)) {
            memcpy(&cpuTemp, datagram.constData(), sizeof(cpuTemp));
            memcpy(&cpuVolt, datagram.constData() + sizeof(cpuVolt), sizeof(cpuVolt));
            float temp = XAdcPs_RawToTemperature(cpuTemp);
            float volt = XAdcPs_RawToVoltage(cpuVolt);
//            qDebug() << "get zynq return temp: " << temp;
            QString tempStr = "测试平台处理器核心温度: " +  QString::number(temp,10,3) +
                                "\u2103\n测试平台处理器核心电压: " + QString::number(volt,10,3) + "V";
            qDebug() << tempStr;
            emit sendTemp(tempStr);
        }
    }
}

void UdpWorker::handleGetAutoDetect() {
    current_command = CMD_SET_ZYNQ_AUTODETECT;
    QByteArray data;
    data.append((char)((current_command >> 24) & 0xFF));
    data.append((char)((current_command >> 16) & 0xFF));
    data.append((char)((current_command >> 8) & 0xFF));
    data.append((char)(current_command & 0xFF));
    disconnect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
    udpSocket->writeDatagram(data, QHostAddress(target_ip), remote_port);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    timer.start(5000); // 设置超时时间为1秒
    loop.exec();

    if (udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        qDebug() << "get zynq return: " << datagram;
        // todo: 解析datagram，判断是否获取成功
        if(datagram.contains("AutoDetect Set Success")){
            qDebug() << "AutoDetect Success";
        } else if(datagram.contains("AutoDetect Set Fail")){
            qDebug() << "AutoDetect Fail";
        }

        emit autoDetectResult(datagram);

    }
    disconnect(udpSocket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(udpSocket, &QUdpSocket::readyRead,this, &UdpWorker::handleReadyRead);
}

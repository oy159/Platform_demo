//
// Created by oycr on 2025/4/12.
//

#include "InstrumentSourceManager.h"

InstrumentSourceManager::InstrumentSourceManager(QObject *parent) : QObject(parent) {
    n9040B = new KeySightVisa_N9040B();
    sma100B = new RhodeSchwarzVisa_SMA100B();
    ks33622A = new KeySightVisa_33622A();
    ks34460A = new KSVisa_34460A();
    // tcpInstrument = new TCPInstrument();

    // qDebug() << "Connected to TCP instrument: " << tcpInstrument->getID().c_str();

    Instrument_Resources.clear();
    Instrument_Resources = findAllVisaResources();

    // ks33622A->setFrequency(1, 1000.0); // 设置默认频率为1000Hz
}

InstrumentSourceManager::~InstrumentSourceManager() {
    
}

bool InstrumentSourceManager::connectToN9040B(const std::string &VisaAddress) {
    bool status = n9040B->connect(VisaAddress);
    if (status) {
        qDebug() << "Connected to N9040B: " << n9040B->getID().c_str();
        emit ConnectInstrumentSuccess(InstrumentType::N9040B);
    }else{
        qDebug() << "Failed to connect to N9040B";
        emit ConnectInstrumentFail(InstrumentType::N9040B);
    }

    // bool status = tcpInstrument->connect("127.0.0.1", 5025); // 连接到本地的TCP仪器，端口5025
    // tcpInstrument->sendCommandWrite("*IDN?"); // 发送查询命令

    return status;
}

void InstrumentSourceManager::disconnectFromN9040B() {
    n9040B->disconnect();
}

bool InstrumentSourceManager::connectToSMA100B(const std::string &VisaAddress) {
    bool status = sma100B->connect(VisaAddress);
    if (status) {
        qDebug() << "Connected to: " << sma100B->getID().c_str();
        emit  ConnectInstrumentSuccess(InstrumentType::SMA100B);
    } else {
        qDebug() << "Failed to connect to SMA100B";
        emit ConnectInstrumentFail(InstrumentType::SMA100B);
    }
    
    return status;
}

void InstrumentSourceManager::disconnectFromSMA100B() {
    sma100B->disconnect();
}

bool InstrumentSourceManager::connectTo34460A(const std::string &VisaAddress)
{
    bool status = ks34460A->connect(VisaAddress);
    if (status) {
        qDebug() << "Connected to 34460A: " << ks34460A->getID().c_str();
        emit ConnectInstrumentSuccess(InstrumentType::KS34460A);
    } else {
        qDebug() << "Failed to connect to 34460A";
        emit ConnectInstrumentFail(InstrumentType::KS34460A);
    }

    return status;
}

double InstrumentSourceManager::read34460AVoltage() {
        double voltage = ks34460A->readVoltage();
        qDebug() << "Read Voltage from 34460A:" << voltage;
        return voltage;
}


void InstrumentSourceManager::disconnectFrom34460A() {
    ks34460A->disconnect();
    qDebug() << "Disconnected from 34460A";
}

bool InstrumentSourceManager::connectTo3362A(const std::string &VisaAddress) {
    bool status = ks33622A->connect(VisaAddress);
    if (status) {
        qDebug() << "Connected to " << ks33622A->getID().c_str();
        // ks33622A->setFrequency(1,123456);
        // ks33622A->setFunc(1,"SIN");
        // ks33622A->setVoltage(1,1.0);
        emit  ConnectInstrumentSuccess(InstrumentType::KS3362A);
    } else {
        qDebug() << "Failed to connect to 33622A";
        emit ConnectInstrumentFail(InstrumentType::KS3362A);
    }
    
    return status;
}

void InstrumentSourceManager::disconnectFrom3362A() {
    ks33622A->disconnect();
}


void InstrumentSourceManager::readSA() {
    if (n9040B->m_session == VI_NULL) {
        qDebug() << "N9040B is not connected";
        return;
    }
    QList<QPointF> data;
    try {
        data = n9040B->readSA();
        emit TransferN9040BData(data);
    } catch (const std::runtime_error &e) {
        qDebug() << "Error reading SA data:" << e.what();
    }
}


void InstrumentSourceManager::dynamicDacInstrumentsControl(dynamicDACTestStep step, int fundFreq){
    switch (step) {
        case CaculateSFDR:
            qDebug() << "Caculating SFDR";
            // todo:
            // 获取基频具体位置和幅度
            peakFreq.clear();
            peakAmp.clear();

            n9040B->defineStartFreq(fundFreq - 1e5);
            n9040B->defineStopFreq(fundFreq + 1e5);
            n9040B->defineRBW(1e3);
            n9040B->defineVBW(1e4);
            n9040B->defineRFAttenuation(10);
            n9040B->defineRefLevel(0);
            QThread::msleep(100);
            n9040B->peakSearch(PeakSearchMode::PeakSearch);
            peakFreq.push_back(n9040B->readMarker1Freq());
            peakAmp.push_back(n9040B->readMarker1Amp() + 10);

            for(int i = 2; i <= HARMONIC_NUMBER; i++){
                n9040B->defineStartFreq(fundFreq * i - 1e5);
                n9040B->defineStopFreq(fundFreq * i + 1e5);
                n9040B->defineRBW(1e3);
                n9040B->defineVBW(1e4);
                n9040B->defineRFAttenuation(0);
                n9040B->defineRefLevel(0);
                QThread::msleep(100);
                n9040B->setMarker1X(fundFreq * i);
                n9040B->peakSearch(PeakSearchMode::PeakSearch);
                peakFreq.push_back(n9040B->readMarker1Freq());
                peakAmp.push_back(n9040B->readMarker1Amp()); // 10dB的衰减
                qDebug() << "Harmonic" << i << "Frequency:" << peakFreq[i-1] << "Amplitude:" << peakAmp[i-1];
            }
            Harmonic_Finished = true;
            break;
        case CaculateTHD:
            qDebug() << "Caculating THD";
            // todo:
            if(!Harmonic_Finished){
                peakFreq.clear();
                peakAmp.clear();

                n9040B->defineStartFreq(fundFreq - 1e5);
                n9040B->defineStopFreq(fundFreq + 1e5);
                n9040B->defineRBW(1e3);
                n9040B->defineVBW(1e4);
                n9040B->defineRFAttenuation(10);
                n9040B->defineRefLevel(0);
                QThread::msleep(100);
                n9040B->peakSearch(PeakSearchMode::PeakSearch);
                peakFreq.push_back(n9040B->readMarker1Freq());
                peakAmp.push_back(n9040B->readMarker1Amp() + 10);

                for(int i = 2; i <= HARMONIC_NUMBER; i++){
                    n9040B->defineStartFreq(fundFreq * i - 1e5);
                    n9040B->defineStopFreq(fundFreq * i + 1e5);
                    n9040B->defineRBW(1e3);
                    n9040B->defineVBW(1e4);
                    n9040B->defineRFAttenuation(0);
                    n9040B->defineRefLevel(0);
                    QThread::msleep(100);
                    n9040B->setMarker1X(fundFreq * i);
                    n9040B->peakSearch(PeakSearchMode::PeakSearch);
                    peakFreq.push_back(n9040B->readMarker1Freq());
                    peakAmp.push_back(n9040B->readMarker1Amp()); // 10dB的衰减
                    qDebug() << "Harmonic" << i << "Frequency:" << peakFreq[i-1] << "Amplitude:" << peakAmp[i-1];
                }
                Harmonic_Finished = true;
            }
            
            double thd = peakAmp[0] / (std::accumulate(peakAmp.begin() + 1, peakAmp.begin() + HARMONIC_NUMBER, 0.0));
            qDebug() << "Thd is" << thd;
            break;
        case CaculateIMD:
            qDebug() << "Caculating IMD";
            // todo:
            break;
        default:
            qDebug() << "Unknown dynamic DAC test step";
            break;
    }
}





std::vector<std::string> InstrumentSourceManager::findAllVisaResources() {
    std::vector<std::string> resources;

    ViSession defaultRM;
    ViStatus status = viOpenDefaultRM(&defaultRM);
    if (status < VI_SUCCESS) {
        qDebug() << "Error opening VISA resource manager:" << status;
        return resources;
    }

    ViFindList findList;
    ViUInt32 retCnt;
    ViChar desc[VI_FIND_BUFLEN];

    // 查找所有仪器资源
    status = viFindRsrc(defaultRM, "?*INSTR", &findList, &retCnt, desc);
    if (status < VI_SUCCESS) {
        qDebug() << "No VISA resources found:" << status;
        viClose(defaultRM);
        return resources;
    }

    qDebug() << "Found" << retCnt << "VISA resources:";

    // 添加第一个找到的资源
    resources.push_back(desc);
    qDebug() << "1:" << desc;

    // 继续查找其余资源
    for (ViUInt32 i = 1; i < retCnt; i++) {
        status = viFindNext(findList, desc);
        if (status < VI_SUCCESS) {
            qDebug() << "Error finding next resource:" << status;
            break;
        }
        resources.push_back(desc);
        qDebug() << i+1 << ":" << desc;
    }

    // 清理资源
    viClose(findList);
    viClose(defaultRM);

    return resources;
}
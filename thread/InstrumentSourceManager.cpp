//
// Created by oycr on 2025/4/12.
//

#include "InstrumentSourceManager.h"

InstrumentSourceManager::InstrumentSourceManager(QObject *parent) : QObject(parent) {
    n9040B = new KeySightVisa_N9040B();
    sma100B = new RhodeSchwarzVisa_SMA100B();

    Instrument_Resources.clear();
    Instrument_Resources = findAllVisaResources();
}

InstrumentSourceManager::~InstrumentSourceManager() {

}

void InstrumentSourceManager::connectToN9040B(const std::string &VisaAddress) {
    n9040B->connect(VisaAddress);
    qDebug() << "Connected to N9040B: " << n9040B->getID().c_str();
}

void InstrumentSourceManager::disconnectFromN9040B() {
        n9040B->disconnect();
}

void InstrumentSourceManager::connectToSMA100B(const std::string &VisaAddress) {
    sma100B->connect(VisaAddress);
    qDebug() << "Connected to SMA100B: " << sma100B->getID().c_str();
}

void InstrumentSourceManager::disconnectFromSMA100B() {
    sma100B->disconnect();
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
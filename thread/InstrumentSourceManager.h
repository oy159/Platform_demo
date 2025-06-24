//
// Created by oycr on 2025/4/12.
//

#ifndef PLATFORM_DEMO_INSTRUMENTSOURCEMANAGER_H
#define PLATFORM_DEMO_INSTRUMENTSOURCEMANAGER_H

#include "QObject"
#include "KeySightVisa_N9040B.h"
#include "RhodeSchwarzVisa_SMA100B.h"
#include "KSVisa_33622A.h"
#include "TCPInstrument.h"
#include "QDebug"

class InstrumentSourceManager : public QObject{
Q_OBJECT
public:
    explicit InstrumentSourceManager(QObject *parent = nullptr);
    ~InstrumentSourceManager();

    std::vector<std::string> findAllVisaResources();


    KeySightVisa_N9040B     *n9040B;
    RhodeSchwarzVisa_SMA100B *sma100B;
    KeySightVisa_33622A     *ks33622A;
public slots:
    bool connectToN9040B(const std::string &VisaAddress);
    void disconnectFromN9040B();
    bool connectToSMA100B(const std::string &VisaAddress);
    void disconnectFromSMA100B();
    bool connectTo3458A(const std::string &VisaAddress);
    void disconnectFrom3458A();
    bool connectTo3362A(const std::string &VisaAddress);
    void disconnectFrom3362A();

private:
    

    std::vector<std::string> Instrument_Resources;


    TCPInstrument *tcpInstrument;
};


#endif //PLATFORM_DEMO_INSTRUMENTSOURCEMANAGER_H

//
// Created by oycr on 2025/4/12.
//

#ifndef PLATFORM_DEMO_INSTRUMENTSOURCEMANAGER_H
#define PLATFORM_DEMO_INSTRUMENTSOURCEMANAGER_H

#include "QObject"
#include "KeySightVisa_N9040B.h"
#include "RhodeSchwarzVisa_SMA100B.h"
#include "KSVisa_33622A.h"
#include "KSVisa_34460A.h"
#include "TCPInstrument.h"
#include "QDebug"
#include "QThread"
#include <vector>
#include <numeric>


#define HARMONIC_NUMBER 5

typedef enum{
    N9040B = 0,
    KS3362A = 1,
    SMA100B = 2,
    KS34460A = 3,
    UnknownInstrument = 4
} InstrumentType;


typedef enum{
    CaculateSFDR = 0,
    CaculateTHD = 1,
    CaculateIMD = 2,
} dynamicDACTestStep;


class InstrumentSourceManager : public QObject{
Q_OBJECT
public:
    explicit InstrumentSourceManager(QObject *parent = nullptr);
    ~InstrumentSourceManager();

    std::vector<std::string> findAllVisaResources();
    


    KeySightVisa_N9040B     *n9040B;
    RhodeSchwarzVisa_SMA100B *sma100B;
    KeySightVisa_33622A     *ks33622A;
    KSVisa_34460A          *ks34460A;

signals:
    void ConnectInstrumentSuccess(InstrumentType instrument);
    void ConnectInstrumentFail(InstrumentType instrument);
    void TransferN9040BData(const QList<QPointF> data);

public slots:
    bool connectToN9040B(const std::string &VisaAddress);
    void disconnectFromN9040B();
    void readSA();

    bool connectToSMA100B(const std::string &VisaAddress);
    void disconnectFromSMA100B();

    bool connectTo34460A(const std::string &VisaAddress);
    void disconnectFrom34460A();
    double read34460AVoltage();

    bool connectTo3362A(const std::string &VisaAddress);
    void disconnectFrom3362A();
    

    void dynamicDacInstrumentsControl(dynamicDACTestStep step = CaculateSFDR, int fundFreq = 1e7);
    
    

private:
    

    std::vector<std::string> Instrument_Resources;


    TCPInstrument *tcpInstrument;

    std::vector<double> peakFreq;
    std::vector<double> peakAmp;
    bool Harmonic_Finished = false;
};


#endif //PLATFORM_DEMO_INSTRUMENTSOURCEMANAGER_H

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


void InstrumentSourceManager::readSA(int freq) {
    if (n9040B->m_session == VI_NULL) {
        qDebug() << "N9040B is not connected";
        return;
    }

    n9040B->defineStartFreq(freq / 10);
    n9040B->defineStopFreq(freq*10);
    n9040B->defineRBW(1e5);
    n9040B->defineVBW(1e5);
    n9040B->defineRFAttenuation(0);
    n9040B->defineRefLevel(0);
    QThread::msleep(1000);
    QList<QPointF> data;
    try {
        data = n9040B->readSA();
        n9040B->defineContinuous(true);
        emit TransferN9040BData(data);
    } catch (const std::runtime_error &e) {
        qDebug() << "Error reading SA data:" << e.what();
    }
}


void InstrumentSourceManager::dynamicDacInstrumentsControl(dynamicDACTestStep step, int fundFreq){
    int offsetFreq = 500;
    double refLevel = 0;
    const int RBW = 10;
    const int VBW = 10;

    auto configureSpectrumAnalyzer = [&](int centerFreq) {
        n9040B->defineStartFreq(centerFreq - offsetFreq);
        n9040B->defineStopFreq(centerFreq + offsetFreq);
        n9040B->defineRBW(RBW);
        n9040B->defineVBW(VBW);
        n9040B->defineRFAttenuation(0);
        n9040B->defineRefLevel(refLevel);
        QThread::msleep(2000); // Allow settling time
    };

    auto measurePeak = [&](int expectedFreq = 0) {
        if (expectedFreq > 0) {
            n9040B->setMarker1X(expectedFreq);
        }
        n9040B->peakSearch(PeakSearchMode::PeakSearch);
        return std::make_pair(n9040B->readMarker1Freq(), n9040B->readMarker1Amp());
    };

    switch (step) {
        case CaculateSFDR: {
            qDebug() << "Calculating SFDR";
            peakFreq.clear();
            peakAmp.clear();

            // Measure fundamental frequency
            configureSpectrumAnalyzer(fundFreq);
            auto [freq, amp] = measurePeak();
            peakFreq.push_back(freq);
            peakAmp.push_back(amp);

            // Measure harmonics
            for (int i = 2; i <= HARMONIC_NUMBER; i++) {
                configureSpectrumAnalyzer(fundFreq * i);
                auto [harmonicFreq, harmonicAmp] = measurePeak();
                peakFreq.push_back(harmonicFreq);
                peakAmp.push_back(harmonicAmp);
                qDebug() << "Harmonic" << i << "Frequency:" << harmonicFreq << "Amplitude:" << harmonicAmp;
            }

            Harmonic_Finished = true;
            auto maxHarmonicAmp = *std::max_element(peakAmp.begin() + 1, peakAmp.end());
            double sfdr = peakAmp[0] - maxHarmonicAmp;
            qDebug() << "SFDR is" << sfdr << "dB";
            break;
        }

        case CaculateTHD: {
            qDebug() << "Calculating THD";

            if (!Harmonic_Finished) {
                peakFreq.clear();
                peakAmp.clear();

                // Measure fundamental frequency
                configureSpectrumAnalyzer(fundFreq);
                auto [freq, amp] = measurePeak();
                peakFreq.push_back(freq);
                peakAmp.push_back(amp);

                // Measure harmonics
                for (int i = 2; i <= HARMONIC_NUMBER; i++) {
                    configureSpectrumAnalyzer(fundFreq * i);
                    auto [harmonicFreq, harmonicAmp] = measurePeak();
                    peakFreq.push_back(harmonicFreq);
                    peakAmp.push_back(harmonicAmp);
                    qDebug() << "Harmonic" << i << "Frequency:" << harmonicFreq << "Amplitude:" << harmonicAmp;
                }
                Harmonic_Finished = true;
            }

            // Convert dBm to linear power (mW) for THD calculation
            double fundamentalPower = std::pow(10.0, peakAmp[0] / 10.0);
            double sumHarmonicPower = 0.0;

            for (size_t i = 1; i < peakAmp.size(); i++) {
                sumHarmonicPower += std::pow(10.0, peakAmp[i] / 10.0);
            }

            double thd = 100.0 * std::sqrt(sumHarmonicPower / fundamentalPower); // THD in percentage
            qDebug() << "THD is" << thd << "%";
            break;
        }

        case CaculateIMD: {
            qDebug() << "Calculating IMD";
            const int freq1 = fundFreq;
            const int freq2 = fundFreq + 1e6;

            // Calculate IMD frequencies
            ImdFreq = {
                    freq1,                      // Fundamental 1
                    freq2,                      // Fundamental 2
                    freq2 - freq1,              // Difference tone (2nd order)
                    freq1 + freq2,              // Sum tone (2nd order)
                    2 * freq2 - freq1,          // 3rd order IMD (lower)
                    2 * freq1 - freq2,          // 3rd order IMD (upper)
                    2 * freq2 + freq1,          // 3rd order sum (usually not measured)
                    2 * freq1 + freq2           // 3rd order sum (usually not measured)
            };

            ImdAmp.clear();

            // Measure all IMD components
            for (int freq : ImdFreq) {
                configureSpectrumAnalyzer(freq);
                auto [measuredFreq, amp] = measurePeak(freq);
                ImdAmp.push_back(amp);
                qDebug() << "IMD Component at" << freq << "Hz: Amplitude =" << amp << "dBm";
            }

            // Calculate IMD3 (3rd order intermodulation distortion)
            // Typically calculated as the average of the two 3rd order products
            double imd3 = *std::max_element(ImdAmp.begin() + 4, ImdAmp.end());

            // Calculate IMD relative to fundamentals
            double fundAvg = (ImdAmp[0] + ImdAmp[1]) / 2.0;
            double imd3Relative = fundAvg - imd3;

//            qDebug() << "IMD3 products at" << ImdFreq[4] << "Hz and" << ImdFreq[5] << "Hz";
            qDebug() << "Max IMD3 level:" << imd3 << "dBm";
            qDebug() << "IMD3 relative to fundamentals:" << imd3Relative << "dB";
            break;
        }

        default: {
            qDebug() << "Unknown dynamic DAC test step";
            break;
        }
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
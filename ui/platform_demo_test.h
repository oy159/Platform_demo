#ifndef PLATFORM_DEMO_PLATFORM_DEMO_TEST_H
#define PLATFORM_DEMO_PLATFORM_DEMO_TEST_H

#include <QMainWindow>
#include <QThread>
#include "udpworker.h"
#include "ThreadPool.h"
#include "CaculateParams.h"
#include "SpectrumChartWidget.h"
#include "InstrumentSourceManager.h"
#include "testdata.h"

typedef enum{
    ADC_DYNAMIC_MODE = 0,
    ADC_STATIC_MODE = 1,
    DAC_DYNAMIC_MODE = 2,
    DAC_STATIC_MODE = 3,
    AUTO_CALI_MODE = 4,
} CACULATE_MODE;

typedef enum{
    ForDynamicADC = 0,
    ForStaticADC = 1,
}CALIVOLTAGE_MODE;


QT_BEGIN_NAMESPACE
namespace Ui { class platform_demo_test; }
QT_END_NAMESPACE

class platform_demo_test : public QMainWindow {
Q_OBJECT

public:
    explicit platform_demo_test(QWidget *parent = nullptr);
    ~platform_demo_test() override;

signals:
    void clearADCData();
    void startOtherCaculate(CALIVOLTAGE_MODE CaliVoltageMode);

private slots:
    void handleInitializePlatform(const QString &DeviceName);
    void handleConnectButton();
    void handleInstrumentDetectBtn();
    void handleErrorOccurred(const QString &error);
    void handleDynamicCaculateFinished(double SFDR, double THD, double SNR, double ENOB);
    void handleStaticCaculateFinished(double maxDNL, double maxINL, 
                                       double minDNL, double minINL,
                                       double staticOffset, double staticPeak);
    void handleADCDataCaculate(std::vector<uint16_t> data);
    void handleDynamicADCTest();
    void handleStaticADCTest();
    void handleADCTestAfterCali(CALIVOLTAGE_MODE CaliVoltageMode);
    
    void handleSetConfigForGenerator();

private:
    QVector<double> generateWaveformData(int count);
    void AutoCaliGeneratorVoltage(CALIVOLTAGE_MODE mode);


    Ui::platform_demo_test *ui;

    QThread *mUdpThread;
    UdpWorker *mUdpWorker;

    QThread *mCalculateThread;
    CaculateParams *mCaculateParams;

    QThread *mInstrumentManagerThread;
    InstrumentSourceManager *mInstrumentManager;

    CACULATE_MODE mCaculateMode;
    uint32_t staticDataSize = 0;

    SpectrumChartWidget *chartWidget1;
    SpectrumChartWidget *chartWidget2;
    SpectrumChartWidget *chartWidget3;
    SpectrumChartWidget *chartWidget4;
    SpectrumChartWidget *chartWidget5;

    InstrumentSourceManager *mInstrumentSourceManager;
    InstrumentType mADCUsedInstrumentType = UnknownInstrument;
    DeviceType mDeviceType = UnknownDevice;
    CALIVOLTAGE_MODE mCaliVoltageMode = ForDynamicADC;
    std::vector<bool> mCaliVoltageFlag = {false, false};
    std::vector<bool> mConnectToInstrumentFlag = {false, false, false, false}; // N9040B, KS3362A, SMA100B, KS34460A

    double sma100b_amp = 0;
    double ks3362a_volt = 1.2;
    bool mUdpStartFlag = false;
};




#endif //PLATFORM_DEMO_PLATFORM_DEMO_TEST_H
#ifndef PLATFORM_DEMO_PLATFORM_DEMO_TEST_H
#define PLATFORM_DEMO_PLATFORM_DEMO_TEST_H

#include <fstream>
#include <vector>
#include <iomanip> // 用于精度控制
#include <QMainWindow>
#include <QThread>
#include "udpworker.h"
#include "ThreadPool.h"
#include "CaculateParams.h"
#include "SpectrumChartWidget.h"
#include "InstrumentSourceManager.h"
#include "testdata.h"
#include "SpectrumChartTryWidget.h"


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
    void TransferDACStaticData(const std::vector<double>& data);

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
    void handleStaticDACTest();
    void handleDACSetValueSuccess();

    void handleDynamicDACTest();

    void handleStaticDACParamsCalculateFinished(double maxDNL, double maxINL, double minDNL,
                                               double minINL, double staticGain, double staticOffset);

private:
    QVector<double> generateWaveformData(int count);
    void AutoCaliGeneratorVoltage(CALIVOLTAGE_MODE mode);
    static uint16_t get_sorted_index_optimal(uint16_t input);
    static bool writeDACDataToCSV(const std::vector<double>& data,
                      const std::string& filename,
                      int precision = 6);


    Ui::platform_demo_test *ui;

    QThread *mUdpThread;
    UdpWorker *mUdpWorker;

    QThread *mCalculateThread;
    CaculateParams *mCaculateParams;

    QThread *mInstrumentManagerThread;
    InstrumentSourceManager *mInstrumentManager;

    CACULATE_MODE mCaculateMode;
    uint32_t staticDataSize = 0;

    SpectrumChartTryWidget *chartWidget1;
    BaseChartWidget *chartWidget2;
    BaseChartWidget *chartWidget3;
    BaseChartWidget *chartWidget4;
    BaseChartWidget *chartWidget5;
    BaseChartWidget *chartWidget6;
    BaseChartWidget *chartWidget7; 

    InstrumentSourceManager *mInstrumentSourceManager;
    InstrumentType mADCUsedInstrumentType = UnknownInstrument;
    DeviceType mDeviceType = UnknownDevice;
    CALIVOLTAGE_MODE mCaliVoltageMode = ForDynamicADC;
    std::vector<bool> mCaliVoltageFlag = {false, false};
    std::vector<bool> mConnectToInstrumentFlag = {false, false, false, false}; // N9040B, KS3362A, SMA100B, KS34460A

    double sma100b_amp = 0;
    double ks3362a_volt = 0.6;
    bool mUdpStartFlag = false;
    int dac_value = 0;
    std::vector<double> DACStaticData;
};




#endif //PLATFORM_DEMO_PLATFORM_DEMO_TEST_H
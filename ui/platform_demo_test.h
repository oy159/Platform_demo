#ifndef PLATFORM_DEMO_PLATFORM_DEMO_TEST_H
#define PLATFORM_DEMO_PLATFORM_DEMO_TEST_H

#include <QMainWindow>
#include <QThread>
#include "udpworker.h"
#include "ThreadPool.h"
#include "CaculateParams.h"
#include "ChartWidget.h"
#include "InstrumentSourceManager.h"

typedef enum{
    ADC_DYNAMIC_MODE = 0,
    ADC_STATIC_MODE = 1,
    DAC_DYNAMIC_MODE = 2,
    DAC_STATIC_MODE = 3,
} CACULATE_MODE;


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

private slots:
    void handleConnectButton();
    void handleSendButton();
    void handleErrorOccurred(const QString &error);
    void handleCaculateFinished(double SFDR, double THD, double SNR, double ENOB);
    void handleADCDataCaculate(std::vector<uint16_t> data);
    void handleDynamicADCTest();
    void handleStaticADCTest();

private:
    QVector<double> generateWaveformData(int count);


    Ui::platform_demo_test *ui;

    QThread *mUdpThread;
    UdpWorker *mUdpWorker;

    QThread *mCalculateThread;
    CaculateParams *mCaculateParams;

    QThread *mInstrumentManagerThread;
    InstrumentSourceManager *mInstrumentManager;

    CACULATE_MODE mCaculateMode;
    bool adcStaticTestStop = false;
    uint32_t staticDataSize = 0;

    ChartWidget *chartWidget1;

    InstrumentSourceManager *mInstrumentSourceManager;
};




#endif //PLATFORM_DEMO_PLATFORM_DEMO_TEST_H
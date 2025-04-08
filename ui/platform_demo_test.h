#ifndef PLATFORM_DEMO_PLATFORM_DEMO_TEST_H
#define PLATFORM_DEMO_PLATFORM_DEMO_TEST_H

#include <QMainWindow>
#include <QThread>
#include "udpworker.h"
#include "ThreadPool.h"
#include "CaculateParams.h"
#include "ChartWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class platform_demo_test; }
QT_END_NAMESPACE

class platform_demo_test : public QMainWindow {
Q_OBJECT

public:
    explicit platform_demo_test(QWidget *parent = nullptr);
    ~platform_demo_test() override;

private slots:
    void handleConnectButton();
    void handleSendButton();
    void handleMessageReceived(const QByteArray &message);
    void handleErrorOccurred(const QString &error);
    void handleCaculateFinished(double SFDR, double THD, double SNR, double ENOB);
    void handleShowChartPanel1();
    void handleRefreshChart1(std::vector<double> data);

private:
    QVector<double> generateWaveformData(int count);


    Ui::platform_demo_test *ui;

    QThread *mUdpThread;
    std::vector<uint16_t> AdcDataArray;
    UdpWorker *mUdpWorker;

    QThread *mCalculateThread;
    CaculateParams *mCaculateParams;

    ChartWidget *chartWidget1;
};

#endif //PLATFORM_DEMO_PLATFORM_DEMO_TEST_H
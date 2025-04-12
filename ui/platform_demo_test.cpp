#include <QRandomGenerator>
#include "platform_demo_test.h"
#include "ui_platform_demo_test.h"
#include "testdata.h"
#include <vector>


platform_demo_test::platform_demo_test(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::platform_demo_test) {
    ui->setupUi(this);

    connect(ui->connectButton, &QPushButton::clicked, this, &platform_demo_test::handleConnectButton);
    connect(ui->sendButton, &QPushButton::clicked, this, &platform_demo_test::handleSendButton);

    mUdpWorker = new UdpWorker();
    mUdpThread = new QThread(this);
    mUdpWorker->moveToThread(mUdpThread);

    connect(mUdpWorker, &UdpWorker::ADCDataReady, this, &platform_demo_test::handleADCDataCaculate);
    connect(mUdpWorker, &UdpWorker::errorOccurred, this, &platform_demo_test::handleErrorOccurred);
    connect(this, &platform_demo_test::clearADCData, mUdpWorker, &UdpWorker::handleClearADCData);

    mUdpThread->start();

    mCaculateParams = new CaculateParams();
    mCalculateThread = new QThread(this);
    mCaculateParams->moveToThread(mCalculateThread);
    connect(mCaculateParams, &CaculateParams::paramsCalculateFinished, this, &platform_demo_test::handleCaculateFinished);

    // ui->connectSettings = new ConnectSettings();
    // connect(ui->connectSetAction, &QAction::triggered, this, [=]() {
    //     ui->connectSettings->show();
    // });


    connect(ui->switchWidgetButton, &QPushButton::clicked, this, [=]() {
        int currentIndex = ui->rightStackedWidget->currentIndex();
        int nextIndex = (currentIndex + 1) % ui->rightStackedWidget->count();
        ui->rightStackedWidget->setCurrentIndex(nextIndex); 
        // 更新按钮文本
        if (nextIndex == 0) {
            ui->switchWidgetButton->setText("切换到DAC参数");
        } else {
            ui->switchWidgetButton->setText("切换到ADC参数");
        }
    });

    chartWidget1 = ui->chartWidget1;

    connect(mCaculateParams, &CaculateParams::TransferFFTData, chartWidget1, &ChartWidget::handleRefreshSpectrum);
    connect(mCaculateParams, &CaculateParams::TransferPeakData, chartWidget1, &ChartWidget::handleRefreshPeakData);


    mCaculateParams->setData(Adc_data2);
    mCalculateThread->start();
    QMetaObject::invokeMethod(mCaculateParams, "caculateDynamicParamsADC", Qt::QueuedConnection);



    mInstrumentSourceManager = new InstrumentSourceManager();

    connect(ui->dynamicParamsADCTestButton, &QPushButton::clicked, this, &platform_demo_test::handleDynamicADCTest);

    /** test code **/
    // create timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        // 生成一些测试数据
        QVector<double> data = generateWaveformData(32768);  // 你的数据生成函数
        std::vector<double> vecdata(data.begin(), data.end());
        mCaculateParams->setData(vecdata);
        mCalculateThread->start();
        QMetaObject::invokeMethod(mCaculateParams, "caculateDynamicParamsADC", Qt::QueuedConnection);
    });
//    timer->start(1000); // 每秒更新一次
}

platform_demo_test::~platform_demo_test() {
    mUdpThread->quit();
    mUdpThread->wait();
    delete ui;
}

void platform_demo_test::handleConnectButton() {
    if (ui->connectButton->isChecked()) {
        ui->connectButton->setText("连接中...");
        QString ip = ui->connectSettings->ipLineEdit->text();
        int remote_port = ui->connectSettings->portLineEdit->text().toInt();
        int local_port = 11451;
        QMetaObject::invokeMethod(mUdpWorker, "connectToHost", Qt::QueuedConnection, Q_ARG(QString, ip), Q_ARG(int, remote_port), Q_ARG(int, local_port));
    } else {
        ui->connectButton->setText("连接设备");
        // disconnect
        QMetaObject::invokeMethod(mUdpWorker, "disconnectFromHost", Qt::QueuedConnection);
    }
}

void platform_demo_test::handleSendButton() {
//    QString message = "Hello, World!";
//    if (!message.isEmpty()) {
//        QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
//    }
}


void platform_demo_test::handleCaculateFinished(double SFDR, double THD, double SNR, double ENOB) {
    qDebug() << "sfdr_db: " << SFDR;
    qDebug() << "THD: " << THD;
    qDebug() << "SNR: " << SNR;
    qDebug() << "ENOB: " << ENOB;
    ui->SFDRADCLabel->setText("SFDR:   " + QString::number(SFDR, 'f', 2) + " dB");
    ui->THDADCLabel->setText("THD:   " + QString::number(THD, 'f', 2) + " dB");
    ui->SNRADCLabel->setText("SNR:    " + QString::number(SNR, 'f', 2) + " dB");
    ui->ENOBADCLabel->setText("ENOB:  " + QString::number(ENOB, 'f', 2) + " bit");
    mCalculateThread->quit();
    mCalculateThread->wait();
}


void platform_demo_test::handleErrorOccurred(const QString &error) {
    // ui->logTextEdit->append("Error: " + error);
    qDebug() << "Error: " << error;
}


QVector<double> platform_demo_test::generateWaveformData(int count)
{
    QVector<double> data;
    data.reserve(count);
    int snr_db = 60;
    auto amplitude = 1.0;
    double fs = 5e8;
    double fc = 2e7;

    // 设置随机数引擎（正态分布）
    std::random_device rd;
    std::mt19937 gen(rd());

    // 计算噪声标准差
    double signal_power = amplitude * amplitude / 2.0; // 假设正弦信号
    double snr_linear = qPow(10.0, snr_db / 10.0);
    double noise_stddev = qSqrt(signal_power / snr_linear);

    // 正态分布噪声生成器
    std::normal_distribution<double> dist(0.0, noise_stddev);

    // 12位量化参数
    const int bits = 12;
    const double quant_step = 5.0 / ((1 << bits) - 1);

    for (int i = 0; i < count; ++i) {
        double x = i;

        // 生成信号（这里以正弦波为例）
        double signal = amplitude * qSin(2 * M_PI * i * fc / fs);

        // 添加高斯噪声
        double noise = dist(gen);
        double noisy_signal = signal + noise;

        // 12位量化
        double quantized = qRound(noisy_signal / quant_step) + (1 << (bits - 1));

        data.append( quantized);
    }
    return data;
}


void platform_demo_test::handleADCDataCaculate(std::vector<uint16_t> data) {
    // 处理接收到的ADC数据
    std::vector<double> dataArray;
    for (unsigned short i : data) {
        dataArray.push_back((double) i);
    }

    mCaculateParams->setData(dataArray);
    mCalculateThread->start();
    if(mCaculateMode == ADC_DYNAMIC_MODE) {
        QMetaObject::invokeMethod(mCaculateParams, "caculateDynamicParamsADC", Qt::QueuedConnection);
    } else if (mCaculateMode == ADC_STATIC_MODE  && !adcStaticTestStop) {
        QMetaObject::invokeMethod(mCaculateParams, "caculateStaticParamsADC", Qt::QueuedConnection);

        // todo:计算获取的数据总量是否满足置信度要求，决定是否关闭循环


        // todo:再获取一次数据（计算完后执行，signal&slot）

    }
    emit clearADCData();
}

void platform_demo_test::handleDynamicADCTest() {
    // 第一步，验证当前固件为ADC，且已打开udp



    // 2.获取adc数据 计算动态参数
    QString message = "Hello, World!";      // 修改为获取一次ADC数据的命令
    if (!message.isEmpty()) {
        QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
    }
    mCaculateMode = ADC_DYNAMIC_MODE;
}

void platform_demo_test::handleStaticADCTest() {
    // 1.验证当前固件为ADC，且已打开udp


    // todo: 2.获取第一次adc数据


    // todo: 3.设置标志位使计算完成后循环直至达到条件

    mCaculateMode = ADC_STATIC_MODE;
    adcStaticTestStop = false;
}

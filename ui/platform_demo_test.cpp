#include <QRandomGenerator>
#include "platform_demo_test.h"
#include "ui_platform_demo_test.h"
#include "testdata.h"
#include <vector>

#define BIG_LITTLE_SWAP16(x)        ( (((*(short int *)&x) & 0xff00) >> 8) | \
                                      (((*(short int *)&x) & 0x00ff) << 8) )


void platform_demo_test::convertBufferToU16Array(const QByteArray &buffer, std::vector<uint16_t> &u16Array) {
    int size = buffer.size();
    if (size % 2 != 0 ) {
        size -= 1; // Ignore the last byte if the size is odd
        transFinished = true;
    }else if(u16Array.size()*2 + size == 65532*2){
        transFinished = true;
        // emit is_framehead==false
        QMetaObject::invokeMethod(mUdpWorker, "handleTransferFinished", Qt::QueuedConnection);
    }
    for (int i = 0; i < size; i += 2) {
        uint16_t value = (uint16_t)buffer[i] << 8 | (uint16_t)buffer[i + 1];
        u16Array.push_back(BIG_LITTLE_SWAP16(value));
    }
}

platform_demo_test::platform_demo_test(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::platform_demo_test) {
    ui->setupUi(this);

    connect(ui->connectButton, &QPushButton::clicked, this, &platform_demo_test::handleConnectButton);
    connect(ui->sendButton, &QPushButton::clicked, this, &platform_demo_test::handleSendButton);

    mUdpWorker = new UdpWorker();
    mUdpThread = new QThread(this);
    mUdpWorker->moveToThread(mUdpThread);

    connect(mUdpWorker, &UdpWorker::messageReceived, this, &platform_demo_test::handleMessageReceived);
    connect(mUdpWorker, &UdpWorker::errorOccurred, this, &platform_demo_test::handleErrorOccurred);

    mUdpThread->start();

    mCaculateParams = new CaculateParams();
    mCalculateThread = new QThread(this);
    mCaculateParams->moveToThread(mCalculateThread);
    connect(mCaculateParams, &CaculateParams::paramsCalculateFinished, this, &platform_demo_test::handleCaculateFinished);
    connect(mCaculateParams, &CaculateParams::TransferFFTData, this, &platform_demo_test::handleRefreshChart1);


    connect(ui->connectSetAction, &QAction::triggered, this, [=]() {
        if (!ui->connectSettings) {
            ui->connectSettings = new ConnectSettings();
        }
        ui->connectSettings->show();
    });


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

    connect(mCaculateParams, &CaculateParams::TransferPeakData, chartWidget1, &ChartWidget::handleRefreshPeakData);

    mCaculateParams->setData(Adc_data2);
    mCalculateThread->start();
    QMetaObject::invokeMethod(mCaculateParams, "calculateParams", Qt::QueuedConnection);

    /** test code **/
    // create timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        // 生成一些测试数据
        QVector<double> data = generateWaveformData(32768);  // 你的数据生成函数
        std::vector<double> vecdata(data.begin(), data.end());
        mCaculateParams->setData(vecdata);
        mCalculateThread->start();
        QMetaObject::invokeMethod(mCaculateParams, "calculateParams", Qt::QueuedConnection);
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
    QString message = "Hello, World!"; 
    if (!message.isEmpty()) {
        QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
    } else {
        // ui->logTextEdit->append("Message is empty");
        qDebug() << "Message is empty";
    }
}

void platform_demo_test::handleMessageReceived(const QByteArray &message) {
//    ui->logTextEdit->append(message);
    convertBufferToU16Array(message, AdcDataArray);
    if(transFinished){
        std::vector<double> dataArray;
        // 处理函数
        for (int i = 0; i < AdcDataArray.size(); ++i) {
            dataArray.push_back((double) AdcDataArray[i]);
        }

        mCaculateParams->setData(dataArray);
        mCalculateThread->start();
        QMetaObject::invokeMethod(mCaculateParams, "calculateParams", Qt::QueuedConnection);

        transFinished = false;
        AdcDataArray.clear();
    }
}

void platform_demo_test::handleCaculateFinished(double SFDR, double THD, double SNR, double ENOB) {
    qDebug() << "sfdr_db: " << SFDR;
    qDebug() << "THD: " << THD;
    qDebug() << "SNR: " << SNR;
    qDebug() << "ENOB: " << ENOB;
    // ui->SFDRADCLabel->setText("SFDR:   " + QString::number(SFDR, 'f', 2) + " dB");
    // ui->THDADCLabel->setText("THD:   " + QString::number(THD, 'f', 2) + " dB");
    // ui->SNRADCLabel->setText("SNR:    " + QString::number(SNR, 'f', 2) + " dB");
    // ui->ENOBADCLabel->setText("ENOB:  " + QString::number(ENOB, 'f', 2) + " bit");
    mCalculateThread->quit();
    mCalculateThread->wait();
}


void platform_demo_test::handleErrorOccurred(const QString &error) {
    // ui->logTextEdit->append("Error: " + error);
    qDebug() << "Error: " << error;
}


void platform_demo_test::handleShowChartPanel1() {
    if (!chartWidget1) {
        chartWidget1 = new ChartWidget(this);
    }

    // 如果窗口已显示则激活，否则显示
    if (chartWidget1->isVisible()) {
        chartWidget1->activateWindow();
        chartWidget1->raise();
    } else {
        chartWidget1->show();
    }
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


void platform_demo_test::handleRefreshChart1(std::vector<double> data) {
    QVector<QPointF> chart_data;
    for (size_t i = 0; i < data.size(); ++i) {
        chart_data.append(QPointF(i, data[i]));
    }

    chartWidget1->updateWaveformData(chart_data);
}


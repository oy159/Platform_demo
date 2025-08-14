#include <QRandomGenerator>
#include "platform_demo_test.h"
#include "ui_platform_demo_test.h"
#include <vector>

static std::vector<double> readCSVData(const std::string& filename) {
    std::vector<double> data;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return data;
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double value;
        if (iss >> value) {
            data.push_back(value);
        }
    }

    file.close();
    return data;
}

platform_demo_test::platform_demo_test(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::platform_demo_test) {
    setWindowTitle("AD/DA 测试平台");
    ui->setupUi(this);
    setStyleSheet("QGroupBox {"
                  "font-weight: bold;"
                  "border: 1px solid #ccc;"
                  "margin-top: 10px;"
                  "}"
                  "QGroupBox::title {"
                  "subcontrol-origin: margin;"
                  "subcontrol-position: top left;"
                  "padding: 0 3px;"
                  "}"
                  "QPushButton {"
                  "background-color: #ffffff;"
                  "border: 1px solid #ccc;"
                  "padding: 5px;"
                  "border-radius: 6px;"  // Add this line to make buttons rounded
                  "}"
                  "QPushButton:hover {"
                  "background-color: #a0e0e0;"
                  "}"
                  "QComboBox {"
                  "padding: 3px;"
                  "border: 1px solid #ccc;"
                  "combobox-popup:0;"
                  "}"
                  );

    ui->connectSettings->setStyleSheet("QGroupBox {"
                  "font-weight: bold;"
                  "border: 1px solid #ccc;"
                  "margin-top: 10px;"
                  "}"
                  "QGroupBox::title {"
                  "subcontrol-origin: margin;"
                  "subcontrol-position: top left;"
                  "padding: 0 3px;"
                  "}"
                  "QPushButton {"
                  "background-color: #ffffff;"
                  "border: 1px solid #ccc;"
                  "padding: 5px;"
                  "border-radius: 6px;"  // Add this line to make buttons rounded
                  "}"
                  "QPushButton:hover {"
                  "background-color: #a0e0e0;"
                  "}"
                  "QComboBox {"
                  "padding: 3px;"
                  "border: 1px solid #ccc;"
                  "combobox-popup:0;"
                  "}"
                  );

    connect(this, &platform_demo_test::startOtherCaculate, this, &platform_demo_test::handleADCTestAfterCali);

    connect(ui->connectSetAction, &QAction::triggered, this, [=]() {
        ui->connectSettings->show();
    });

    connect(ui->connectSettings->SettingGeneratorResourceBtn, &QPushButton::clicked, this, &platform_demo_test::handleSetConfigForGenerator);

    connect(ui->connectButton, &QPushButton::clicked, this, &platform_demo_test::handleConnectButton);
    connect(ui->instrumentDetectBtn, &QPushButton::clicked, this, &platform_demo_test::handleInstrumentDetectBtn);

    mUdpWorker = new UdpWorker();
    mUdpThread = new QThread(this);
    mUdpWorker->moveToThread(mUdpThread);

    connect(mUdpWorker, &UdpWorker::ADCDataReady, this, &platform_demo_test::handleADCDataCaculate);
    connect(mUdpWorker, &UdpWorker::errorOccurred, this, &platform_demo_test::handleErrorOccurred);
    connect(mUdpWorker, &UdpWorker::initializePlatform, this, &platform_demo_test::handleInitializePlatform);

    connect(this, &platform_demo_test::clearADCData, mUdpWorker, &UdpWorker::handleClearADCData);

    mUdpThread->start();

    mCaculateParams = new CaculateParams();
    mCalculateThread = new QThread(this);
    mCaculateParams->moveToThread(mCalculateThread);
    connect(mCaculateParams, &CaculateParams::dynamicParamsCalculateFinished, this, &platform_demo_test::handleDynamicCaculateFinished);
    connect(mCaculateParams, &CaculateParams::staticParamsCalculateFinished, this, &platform_demo_test::handleStaticCaculateFinished);

    mCalculateThread->start();


    mInstrumentSourceManager = new InstrumentSourceManager();
    mInstrumentManagerThread = new QThread(this);
    mInstrumentSourceManager->moveToThread(mInstrumentManagerThread);

    mInstrumentManagerThread->start();

    connect(mInstrumentSourceManager, &InstrumentSourceManager::ConnectInstrumentSuccess, this, [=](InstrumentType instrument) {
        QList<QPointF> SpData;

        switch (instrument) {
            case InstrumentType::N9040B:
                ui->connectSettings->DetectSpectrumBtn->setText("连接成功");
                mConnectToInstrumentFlag[0] = true;
//                QMetaObject::invokeMethod(mInstrumentSourceManager, "readSA",
//                                          Qt::QueuedConnection);
                break;
            case InstrumentType::KS3362A:
                ui->connectSettings->DetectGeneratorBtn2->setText("连接成功");
                mConnectToInstrumentFlag[1] = true;
                mADCUsedInstrumentType = InstrumentType::KS3362A;
//                mInstrumentSourceManager->ks33622A->setTwoTone(1,1);
                break;
            case InstrumentType::SMA100B:
                ui->connectSettings->DetectGeneratorBtn->setText("连接成功");
                mConnectToInstrumentFlag[2] = true;
                mADCUsedInstrumentType = InstrumentType::SMA100B;
                break;
            case InstrumentType::KS34460A:
                ui->connectSettings->DetectVoltmeterBtn->setText("连接成功");
                mConnectToInstrumentFlag[3] = true;
                mInstrumentSourceManager->ks34460A->setNPLCTime(1e-3);
                break;
            default:
                break;
        }
    });

    connect(mInstrumentSourceManager, &InstrumentSourceManager::ConnectInstrumentFail, this, [=](InstrumentType instrument) {
        switch (instrument) {
            case InstrumentType::N9040B:
                ui->connectSettings->DetectSpectrumBtn->setText("连接失败");
                mConnectToInstrumentFlag[0] = false;
                break;
            case InstrumentType::KS3362A:
                ui->connectSettings->DetectGeneratorBtn2->setText("连接失败");
                mConnectToInstrumentFlag[1] = false;

                break;
            case InstrumentType::SMA100B:
                ui->connectSettings->DetectGeneratorBtn->setText("连接失败");
                mConnectToInstrumentFlag[2] = false;
                break;
            case InstrumentType::KS34460A:
                ui->connectSettings->DetectVoltmeterBtn->setText("连接失败");
                mConnectToInstrumentFlag[3] = false;
                break;
            default:
                break;
        }
    });

    // * 检测仪器连接按钮
    connect(ui->connectSettings->DetectSpectrumBtn, &QPushButton::clicked, this, [=]() {
        QString ip = ui->connectSettings->SpectrumResourceLineEdit->text();
        // bool status = mInstrumentSourceManager->connectToN9040B(ip.toStdString());
        QMetaObject::invokeMethod(mInstrumentSourceManager, "connectToN9040B",
                            Qt::QueuedConnection, Q_ARG(std::string, ip.toStdString()));
    });

    connect(ui->connectSettings->DetectGeneratorBtn, &QPushButton::clicked, this, [=]() {
        QString ip = ui->connectSettings->GeneratorResourceLineEdit->text();
        // bool status = mInstrumentSourceManager->connectToSMA100B(ip.toStdString());
        QMetaObject::invokeMethod(mInstrumentSourceManager, "connectToSMA100B",
                            Qt::QueuedConnection, Q_ARG(std::string, ip.toStdString()));
//         if (status) {
//             ui->connectSettings->DetectGeneratorBtn->setText("连接成功");
// //            handleAutoCaliInstrument();
//             mInstrumentSourceManager->sma100B->setExternalClock();
// //            mInstrumentSourceManager->sma100B->SetFrequency((int)
// //                        (1e6*ui->connectSettings->GeneratorResourceFreqSpinBox->value()));
//         } else {
//             ui->connectSettings->DetectGeneratorBtn->setText("连接失败");
//         }
    });

    connect(ui->connectSettings->DetectVoltmeterBtn, &QPushButton::clicked, this, [=]() {
        QString ip = ui->connectSettings->VoltmeterResourceLineEdit->text();
        // bool status = mInstrumentSourceManager->connectTo3458A(ip.toStdString());
        QMetaObject::invokeMethod(mInstrumentSourceManager, "connectTo34460A",
                            Qt::QueuedConnection, Q_ARG(std::string, ip.toStdString()));

    });


    connect(ui->connectSettings->DetectGeneratorBtn2, &QPushButton::clicked, this, [=]() {
        QString ip = ui->connectSettings->GeneratorResourceLineEdit2->text();
        QMetaObject::invokeMethod(mInstrumentSourceManager, "connectTo3362A",
                            Qt::QueuedConnection, Q_ARG(std::string, ip.toStdString()));
    });

    connect(ui->connectSettings->SettingGeneratorResourceBtn2, &QPushButton::clicked, this, &platform_demo_test::handleSetConfigForGenerator);


    // * 切换右侧堆叠窗口
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
    chartWidget2 = ui->chartWidget2;
    chartWidget3 = ui->chartWidget3;
    chartWidget4 = ui->chartWidget4;
    chartWidget5 = ui->chartWidget5;
    chartWidget6 = ui->chartWidget6;
    chartWidget7 = ui->chartWidget7;


    chartWidget3->setAxisPrecision(0,0);
    chartWidget4->setAxisPrecision(0,0);


    connect(mCaculateParams, &CaculateParams::TransferFFTData, chartWidget1, &SpectrumChartTryWidget::handleRefreshSpectrum);
    connect(mCaculateParams, &CaculateParams::TransferPeakData, chartWidget1, &SpectrumChartTryWidget::handleRefreshPeakData);
    connect(ui->FindPeakButton, &QPushButton::clicked, chartWidget1, &SpectrumChartTryWidget::handleFindPeak);
    connect(ui->FindNextButton, &QPushButton::clicked, chartWidget1, &SpectrumChartTryWidget::handleFindNextPeak);

    connect(mCaculateParams, &CaculateParams::TransferADCData, chartWidget2, &BaseChartWidget::updateChartDataDirect);

    connect(mCaculateParams, &CaculateParams::TransferDNLData, chartWidget3, &BaseChartWidget::updateChartDataDirect);
    connect(mCaculateParams, &CaculateParams::TransferINLData, chartWidget4, &BaseChartWidget::updateChartDataDirect);

    connect(mCaculateParams, &CaculateParams::TransferDACDNLData, chartWidget3, &BaseChartWidget::updateChartDataDirect);
    connect(mCaculateParams, &CaculateParams::TransferDACINLData, chartWidget4, &BaseChartWidget::updateChartDataDirect);

    connect(mCaculateParams, &CaculateParams::TransferHistrogramData, chartWidget5, &BaseChartWidget::updateChartDataDirect);
    connect(this, &platform_demo_test::TransferDACStaticData, chartWidget6, &BaseChartWidget::updateChartDataDirect);

    connect(mInstrumentSourceManager, &InstrumentSourceManager::TransferN9040BData, chartWidget7, &BaseChartWidget::updateChartData);

    connect(ui->ADCChannel1CheckBox, &QCheckBox::checkStateChanged, this, [this](int state) {
        if (state == Qt::Checked) {
            ui->ADCChannel2CheckBox->setChecked(false);
            if(mDeviceType == DeviceType::AD9268){
                if(!mUdpStartFlag) {
                    QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
                    return;
                }
                QMetaObject::invokeMethod(mUdpWorker, "handleSetAD9268Channel", Qt::QueuedConnection, Q_ARG(int, 1));
            }else{
                QMessageBox::warning(this, "Warning", "当前不是AD9268！");
            }
        }
    });
    connect(ui->ADCChannel2CheckBox, &QCheckBox::checkStateChanged, this, [this](int state) {
        if (state == Qt::Checked) {
            ui->ADCChannel1CheckBox->setChecked(false);
            if(mDeviceType == DeviceType::AD9268){
                if(!mUdpStartFlag) {
                    QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
                    return;
                }
                QMetaObject::invokeMethod(mUdpWorker, "handleSetAD9268Channel", Qt::QueuedConnection, Q_ARG(int, 2));
            }else{
                QMessageBox::warning(this, "Warning", "当前不是AD9268！");
            }
        }
    });


    connect(ui->DACFrequencySetButton, &QPushButton::clicked, this, [=](){
        int freq = (int)((double)ui->DACFrequencySpinBox->value() * 1e6);
        if (!mUdpStartFlag) {
            QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
            return;
        }
        if(mDeviceType == DeviceType::AD9142 || mDeviceType == DeviceType::AD9747) {
            QMetaObject::invokeMethod(mUdpWorker, "handleSetDDSFreq", Qt::QueuedConnection, Q_ARG(int, freq), Q_ARG(int,0));
        }else{
            QMessageBox::warning(this, "Warning", "当前不是DAC！");
            return;
        }
    });

    connect(ui->staticParamsDACTestButton, &QPushButton::clicked, this, &platform_demo_test::handleStaticDACTest);
    connect(mUdpWorker, &UdpWorker::DACValueSetSuccess, this, &platform_demo_test::handleDACSetValueSuccess);


    connect(ui->dynamicParamsDACTestButton, &QPushButton::clicked, this, &platform_demo_test::handleDynamicDACTest);

//    connect(mInstrumentSourceManager, &InstrumentSourceManager::TransferN9040BData, this, [=](){
//
//    });

//    chartWidget3->adjustaxisX();
    mCaculateParams->setData(adc16Data);
    chartWidget1->setSampleRate(1e8); // 设置采样率

    QMetaObject::invokeMethod(mCaculateParams, "caculateDynamicParamsADC", Qt::QueuedConnection);

    connect(ui->dynamicParamsADCTestButton, &QPushButton::clicked, this, &platform_demo_test::handleDynamicADCTest);
    connect(ui->staticParamsADCTestButton, &QPushButton::clicked, this, &platform_demo_test::handleStaticADCTest);

    connect(mCaculateParams, &CaculateParams::staticDACParamsCalculateFinished, this, &platform_demo_test::handleStaticDACParamsCalculateFinished);
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

    QMetaObject::invokeMethod(mCaculateParams, "setDACStaticData", Qt::QueuedConnection, Q_ARG(vector<double>, readCSVData("DACStaticData.csv")));
}

platform_demo_test::~platform_demo_test() {
    mUdpThread->quit();
    mUdpThread->wait();
    mCalculateThread->quit();
    mCalculateThread->wait();


    delete ui->connectSettings;
    delete ui;
}

void platform_demo_test::handleInitializePlatform(const QString &DeviceName){
    qDebug() << "Initialize platform with device: " << DeviceName;
    if(DeviceName == "AD9434"){
        mCaculateParams->setADCBits(12);
        chartWidget1->setSampleRate(5e8); // 设置采样率
        mDeviceType = DeviceType::AD9434;
        mUdpStartFlag = true;
    }else if(DeviceName == "AD9268"){
        mCaculateParams->setADCBits(16);
        chartWidget1->setSampleRate(1e8); // 设置采样率
        mDeviceType = DeviceType::AD9268;
        mUdpStartFlag = true;
    }else if(DeviceName == "AD9142"){
        mDeviceType = DeviceType::AD9142;
        mUdpStartFlag = true;
    }else if(DeviceName == "AD9747"){
        mDeviceType = DeviceType::AD9747;
        mUdpStartFlag = true;
    }else{
        qDebug() << "Unknown device type!";
        mDeviceType = DeviceType::UnknownDevice;
        mUdpStartFlag = true;
        return;
    }

    ui->connectButton->setText("连接完成");
    ui->connectButton->setStyleSheet("QPushButton {"
                                        "background-color: green;"
                                        "border: 1px solid #ccc;"
                                        "padding: 5px;"
                                        "border-radius: 6px;"  // Add this line to make buttons rounded
                                        "}"
                                        "QPushButton:hover {"
                                        "background-color: #a0e0e0;"
                                        "}");
}

void platform_demo_test::handleConnectButton() {
    if (ui->connectButton->isChecked()) {
        ui->connectButton->setText("连接中...");
        ui->connectButton->setStyleSheet("QPushButton {"
                                            "background-color: gray;"
                                            "border: 1px solid #ccc;"
                                            "padding: 5px;"
                                            "border-radius: 6px;"  // Add this line to make buttons rounded
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: #a0e0e0;"
                                            "}");
        QString ip = ui->connectSettings->ipLineEdit->text();
        int remote_port = ui->connectSettings->portLineEdit->text().toInt();
        int local_port = 11451;
        QMetaObject::invokeMethod(mUdpWorker, "connectToHost", Qt::QueuedConnection, Q_ARG(QString, ip), Q_ARG(int, remote_port), Q_ARG(int, local_port));
    } else {
        ui->connectButton->setText("连接设备");
        ui->connectButton->setStyleSheet("QPushButton {"
                                            "background-color: #ffffff;"
                                            "border: 1px solid #ccc;"
                                            "padding: 5px;"
                                            "border-radius: 6px;"  // Add this line to make buttons rounded
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: #a0e0e0;"
                                            "}");
        // disconnect
        QMetaObject::invokeMethod(mUdpWorker, "disconnectFromHost", Qt::QueuedConnection);
    }
}

void platform_demo_test::handleInstrumentDetectBtn() {
    mInstrumentManager->findAllVisaResources();
}


void platform_demo_test::handleDynamicCaculateFinished(double SFDR, double THD, double SNR, double ENOB) {
    qDebug() << "sfdr_db: " << SFDR;
    qDebug() << "THD: " << THD;
    qDebug() << "SNR: " << SNR;
    qDebug() << "ENOB: " << ENOB;
    ui->SFDRADCLabel->setText("SFDR:   " + QString::number(SFDR, 'f', 2) + " dB");
    ui->THDADCLabel->setText("THD:   " + QString::number(THD, 'f', 2) + " dB");
    ui->SNRADCLabel->setText("SNR:    " + QString::number(SNR, 'f', 2) + " dB");
    ui->ENOBADCLabel->setText("ENOB:  " + QString::number(ENOB, 'f', 2) + " bit");
//    mCalculateThread->quit();
//    mCalculateThread->wait();
}

void platform_demo_test::handleStaticCaculateFinished(double maxDNL, double maxINL, 
                                       double minDNL, double minINL,
                                       double staticOffset, double staticPeak)
{
    qDebug() << "minDNL: " << minDNL;
    qDebug() << "minINL: " << minINL;
    qDebug() << "maxDNL: " << maxDNL;
    qDebug() << "maxINL: " << maxINL;

    ui->DNLADCLabel->setText("DNL:   " + QString::number(minDNL, 'f', 2) + " | " + QString::number(maxDNL, 'f', 2) + " LSB");
    ui->INLADCLabel->setText("INL:   " + QString::number(minINL, 'f', 2) + " | " + QString::number(maxINL, 'f', 2) + " LSB");
    ui->OffsetADCLabel->setText("Offset:   " + QString::number(staticOffset, 'f', 2));
    ui->PeakADCLabel->setText("Peak:   " + QString::number(staticPeak, 'f', 2));


    mCalculateThread->quit();
    mCalculateThread->wait();
}

void platform_demo_test::handleErrorOccurred(const QString &error) {
    // ui->logTextEdit->append("Error: " + error);
    qDebug() << "Error: " << error;
    QMessageBox::warning(this, "Warning", error);
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

void platform_demo_test::handleSetConfigForGenerator() {
    if(!mUdpStartFlag) {
        QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
        return;
    }
    if(ui->connectSettings->GeneratorResourceExternalClockFreqCheckBox->isChecked()){
        if(mDeviceType != AD9268 && mDeviceType != AD9434) {
            QMessageBox::warning(this, "Warning", "当前不是ADC设备，默认不进行外部时钟设置");
            return;
        }
        QMetaObject::invokeMethod(mUdpWorker, "handleSetAD9518ExternalClock",Qt::QueuedConnection, Q_ARG(int, 10000));
        // todo:设置外部时钟设置
        if(mConnectToInstrumentFlag[InstrumentType::SMA100B]) {
            // ? 这样似乎有点危险，在主线程调用方法，可能会遇到未知bug，感觉。。。。
            mInstrumentSourceManager->sma100B->setExternalClock((int)(1e6*ui->connectSettings->GeneratorResourceExternalClockFreqSpinBox->value()));
        }else{
            QMessageBox::warning(this, "Warning", "当前未连接SMA100B仪器，无法进行外部时钟设置！");
            return;
        }
    }else{
        QMetaObject::invokeMethod(mUdpWorker, "handleSetAD9518InternalClock",Qt::QueuedConnection);
    }

    // todo: 设置仪器输出频率
    if(mADCUsedInstrumentType == InstrumentType::SMA100B){
        mInstrumentSourceManager->sma100B->SetFrequency((int)(1e6*ui->connectSettings->GeneratorResourceFreqSpinBox->value()));
    }else if(mADCUsedInstrumentType == InstrumentType::KS3362A) {
        mInstrumentSourceManager->ks33622A->setFrequency(1, (1e6*ui->connectSettings->GeneratorResource2FreqSpinBox->value()));
    }

}


void platform_demo_test::handleADCDataCaculate(std::vector<uint16_t> data) {
    // 处理接收到的ADC数据
    std::vector<double> dataArray;
    for (unsigned short i : data) {
        dataArray.push_back((double) i);
    }

    if(mCaculateMode == AUTO_CALI_MODE){
        auto [minIt, maxIt] = std::minmax_element(dataArray.begin(), dataArray.end());
        double minValue = *minIt;
        double maxValue = *maxIt;

        int minCount = std::count(dataArray.begin(), dataArray.end(), minValue);
        int maxCount = std::count(dataArray.begin(), dataArray.end(), maxValue);
        qDebug() << "Min: " << minValue << ", count: " << minCount;
        qDebug() << "Max: " << maxValue << ", count: " << maxCount;
        if(mADCUsedInstrumentType == InstrumentType::SMA100B){
            qDebug() << "AutoCali test suceessfully" << "Now SMA100B amp is " << sma100b_amp << " dBm";
        }else if(mADCUsedInstrumentType == InstrumentType::KS3362A){
            qDebug() << "AutoCali test suceessfully" << "Now KS3362A volt is " << ks3362a_volt << " V";
        }
        
        
        if(mCaliVoltageMode == CALIVOLTAGE_MODE::ForDynamicADC){
            if(mDeviceType == AD9268){
                if(maxValue > 65535*0.95){ 
                    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForDynamicADC] = true;
                    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForStaticADC] = false;
                }
            }else if(mDeviceType == AD9434){
                if(maxValue > 4095*0.95){
                    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForDynamicADC] = true;
                    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForStaticADC] = false;
                }
            }
        }else if(mCaliVoltageMode == CALIVOLTAGE_MODE::ForStaticADC){
            if(mDeviceType == AD9268){
                if(maxValue == 65535 && maxCount > 3500){
                    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForDynamicADC] = false;
                    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForStaticADC] = true;

                }
            }else if(mDeviceType == AD9434){
                if(maxValue == 4095 && maxCount > 3500){
                    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForDynamicADC] = false;
                    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForStaticADC] = true;
                }
            }
        }

        if(mCaliVoltageFlag[CALIVOLTAGE_MODE::ForDynamicADC] || mCaliVoltageFlag[CALIVOLTAGE_MODE::ForStaticADC]) {
            if(mADCUsedInstrumentType == InstrumentType::SMA100B){
                qDebug() << "AutoCali test suceessfully" << "Now SMA100B amp is " << sma100b_amp << " dBm";
            }else if(mADCUsedInstrumentType == InstrumentType::KS3362A){
                qDebug() << "AutoCali test suceessfully" << "Now KS3362A volt is " << ks3362a_volt << " V";
            }
            qDebug() << "AutoCali test finished, now stop ADC data caculate";
            emit clearADCData();
            emit startOtherCaculate(mCaliVoltageMode);
            return;
        }

        // 调整输出幅度
        if(mADCUsedInstrumentType == InstrumentType::SMA100B){
            sma100b_amp += 0.1;
            mInstrumentSourceManager->sma100B->setAMP(sma100b_amp);
        }else if(mADCUsedInstrumentType == InstrumentType::KS3362A){
            ks3362a_volt += 0.02;
            mInstrumentSourceManager->ks33622A->setVoltage(1, ks3362a_volt);
        }
        emit clearADCData();

        if(mADCUsedInstrumentType == InstrumentType::SMA100B) {
            if (sma100b_amp > 15)
                return;
        }else if(mADCUsedInstrumentType == InstrumentType::KS3362A) {
            if (ks3362a_volt > 3)
                return;
        }


        QString message = "Hello, World!";      // 修改为获取一次ADC数据的命令
        if (!message.isEmpty()) {
            QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
        }

        return;
    }

    mCaculateParams->setData(dataArray);
    mCalculateThread->start();
    if(mCaculateMode == ADC_DYNAMIC_MODE) {
        QMetaObject::invokeMethod(mCaculateParams, "caculateDynamicParamsADC", Qt::QueuedConnection);
        emit clearADCData();
    } else if (mCaculateMode == ADC_STATIC_MODE) {
        ui->staticParamsADCTestButton->setEnabled(false);
        QMetaObject::invokeMethod(mCaculateParams, "caculateStaticDataHistogram", Qt::QueuedConnection);

        staticDataSize += dataArray.size();


        if(staticDataSize >= 65532*1000) {
            qDebug() << "ADC Static Test Stop" << "now data size = " << staticDataSize;
            
            QMetaObject::invokeMethod(mCaculateParams, "caculateStaticParamsADC", Qt::QueuedConnection);

            staticDataSize = 0;
            ui->staticParamsADCTestButton->setEnabled(true);
            emit clearADCData();
            return;
        }
        emit clearADCData();

        QString message = "Hello, World!";      // 修改为获取一次ADC数据的命令
        if (!message.isEmpty()) {
            QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
        }

    }

}

void platform_demo_test::handleDynamicADCTest() {
    // 第一步，验证当前固件为ADC，且已打开udp
    if(!mUdpStartFlag) {
        QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
        return;
    }

    if(mDeviceType ==  UnknownDevice) {
        QMessageBox::warning(this, "Warning", "未知设备！");
        return;
    }

    if(mDeviceType != AD9268 && mDeviceType != AD9434) {
        QMessageBox::warning(this, "Warning", "当前不是ADC设备，无法进行静态指标测试！");
        return;
    }

    AutoCaliGeneratorVoltage(CALIVOLTAGE_MODE::ForDynamicADC);

//    handleADCTestAfterCali(CALIVOLTAGE_MODE::ForDynamicADC);


}

void platform_demo_test::handleStaticADCTest() {
    // 1.验证当前固件为ADC，且已打开udp
    if(!mUdpStartFlag) {
        QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
        return;
    }

    if(mDeviceType ==  UnknownDevice) {
        QMessageBox::warning(this, "Warning", "未知设备！");
        return;
    }

    if(mDeviceType != AD9268 && mDeviceType != AD9434) {
        QMessageBox::warning(this, "Warning", "当前不是ADC设备，无法进行静态指标测试！");
        return;
    }

    AutoCaliGeneratorVoltage(CALIVOLTAGE_MODE::ForStaticADC);
//    handleADCTestAfterCali(CALIVOLTAGE_MODE::ForStaticADC);

}


void platform_demo_test::handleADCTestAfterCali(CALIVOLTAGE_MODE CaliVoltageMode)
{
    if(CaliVoltageMode == CALIVOLTAGE_MODE::ForDynamicADC) {
        mCaculateMode = ADC_DYNAMIC_MODE;
    } else if(CaliVoltageMode == CALIVOLTAGE_MODE::ForStaticADC) {
        mCaculateMode = ADC_STATIC_MODE;
    }
    QString message = "Hello, World!";
    if (!message.isEmpty()) {
        QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
    }
}


void platform_demo_test::AutoCaliGeneratorVoltage(CALIVOLTAGE_MODE mode) {

    mCaliVoltageMode = mode;
    
    if(mADCUsedInstrumentType == InstrumentType::SMA100B) {
        mInstrumentSourceManager->sma100B->SetOutput1Status(false);
        if(mCaliVoltageMode == CALIVOLTAGE_MODE::ForDynamicADC && mCaliVoltageFlag[CALIVOLTAGE_MODE::ForStaticADC]) 
            sma100b_amp = 0;
        mInstrumentSourceManager->sma100B->setAMP(sma100b_amp);        // 0dbm
        mInstrumentSourceManager->sma100B->SetOutput1Status(true);  
    }else if(mADCUsedInstrumentType == InstrumentType::KS3362A) {
        // todo: 设置KS3362A输出幅度为一个较小值
        mInstrumentSourceManager->ks33622A->setOutputStatus(1, false); // 关闭输出
        if(mCaliVoltageMode == CALIVOLTAGE_MODE::ForDynamicADC && mCaliVoltageFlag[CALIVOLTAGE_MODE::ForStaticADC]) 
            ks3362a_volt = 0.6;
        mInstrumentSourceManager->ks33622A->setVoltage(1, ks3362a_volt); // 设置输出电压为1.2V
        mInstrumentSourceManager->ks33622A->setOutputStatus(1, true); // 打开输出
        QThread::msleep(100);
    } else {
        QMessageBox::warning(this, "Warning", "当前未连接SMA100B或KS3362A仪器，无法进行自动校准输出电压功能！");
        return;
    }

    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForDynamicADC] = false;
    mCaliVoltageFlag[CALIVOLTAGE_MODE::ForStaticADC] = false;


    // 2.获取adc数据
    QString message = "Hello, World!";      // 修改为获取一次ADC数据的命令
    if (!message.isEmpty()) {
        QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
    }
    mCaculateMode = AUTO_CALI_MODE;
}

void platform_demo_test::handleStaticDACTest(){
    // todo: 校验DAC固件
    if(!mUdpStartFlag) {
        QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
        return;
    }

    if(mDeviceType ==  UnknownDevice) {
        QMessageBox::warning(this, "Warning", "未知设备！");
        return;
    }

    if(!mConnectToInstrumentFlag[3]){
        QMessageBox::warning(this, "Warning", "台表未连接");
        return;
    }

    if(mDeviceType != AD9142 && mDeviceType != AD9747) {
        QMessageBox::warning(this, "Warning", "当前不是DAC设备，无法进行静态指标测试！");
        return;
    }

    DACStaticData.resize(65536,0);
    dac_value = 0;
    QMetaObject::invokeMethod(mUdpWorker, "handleSetDACValue", Qt::QueuedConnection, Q_ARG(int, dac_value));
}


void platform_demo_test::handleDACSetValueSuccess(){
//     double data = mInstrumentSourceManager->ks34460A->readVoltage();
//    QThread::msleep(2000);
    double data = mInstrumentSourceManager->ks34460A->readDM3068Voltage();
    qDebug() << "Now DAC Value is" << dac_value << " voltage: " << data;
    
    DACStaticData[get_sorted_index_optimal(dac_value)] = data;
    if(dac_value == 65535){
        emit TransferDACStaticData(DACStaticData);
//        mCaculateParams->setDACStaticData(DACStaticData);
        QMetaObject::invokeMethod(mCaculateParams, "setDACStaticData", Qt::QueuedConnection, Q_ARG(vector<double>, DACStaticData));
        if(!writeDACDataToCSV(DACStaticData, "DACStaticData.csv", 6)) {
            QMessageBox::warning(this, "Warning", "写入CSV文件失败！");
        }
        return;
    }
    dac_value++;
    QMetaObject::invokeMethod(mUdpWorker, "handleSetDACValue", Qt::QueuedConnection, Q_ARG(int, dac_value));
}

void platform_demo_test::handleDynamicDACTest(){
    // todo: 动态DAC测试
    int freq = (int)((double)ui->DACFrequencySpinBox->value() * 1e6);
    mUdpWorker->handleSetDDSFreq(freq, 0);
    QThread::msleep(2000);
    QMetaObject::invokeMethod(mInstrumentSourceManager, "dynamicDacInstrumentsControl",
                          Qt::QueuedConnection, Q_ARG(dynamicDACTestStep, dynamicDACTestStep::CaculateSFDR), Q_ARG(int, freq));
//    QMetaObject::invokeMethod(mUdpWorker, "handleSetDDSFreq", Qt::QueuedConnection, Q_ARG(int, freq), Q_ARG(int,1));
    mUdpWorker->handleSetDDSFreq(freq, 1);
    QThread::msleep(2000);
    QMetaObject::invokeMethod(mInstrumentSourceManager, "dynamicDacInstrumentsControl",
                              Qt::QueuedConnection, Q_ARG(dynamicDACTestStep, dynamicDACTestStep::CaculateIMD), Q_ARG(int, freq));
    QMetaObject::invokeMethod(mInstrumentSourceManager, "readSA",
                              Qt::QueuedConnection, Q_ARG(int, freq));

}



uint16_t platform_demo_test::get_sorted_index_optimal(uint16_t input) {
    return static_cast<uint16_t>(static_cast<int16_t>(input) + 32768);
}

bool platform_demo_test::writeDACDataToCSV(const std::vector<double>& data, 
                      const std::string& filename,
                      int precision) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }
    outFile << std::fixed << std::setprecision(precision);

    for (double i : data) {
        outFile << i;
        outFile << "\n"; 
    }
    outFile.close();
    return true;
}

void
platform_demo_test::handleStaticDACParamsCalculateFinished(double maxDNL, double maxINL, double minDNL,
                                                           double minINL, double staticGain, double staticOffset) {
    qDebug() << "minDNL: " << minDNL;
    qDebug() << "minINL: " << minINL;
    qDebug() << "maxDNL: " << maxDNL;
    qDebug() << "maxINL: " << maxINL;
    ui->DNLDACLabel->setText("DNL:   " + QString::number(minDNL, 'f', 2) + " | " + QString::number(maxDNL, 'f', 2) + " LSB");
    ui->INLDACLabel->setText("INL:   " + QString::number(minINL, 'f', 2) + " | " + QString::number(maxINL, 'f', 2) + " LSB");
    ui->OffsetDACLabel->setText("增益:   " + QString::number(staticGain, 'E', 3) + "  V/LSB");
    ui->PeakDACLabel->setText("零点偏移:   " + QString::number(staticOffset, 'E', 3) + "  V");
}

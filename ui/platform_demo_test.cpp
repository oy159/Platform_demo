#include <QRandomGenerator>
#include "platform_demo_test.h"
#include "ui_platform_demo_test.h"
#include <vector>


platform_demo_test::platform_demo_test(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::platform_demo_test) {
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

    


    mInstrumentSourceManager = new InstrumentSourceManager();
    mInstrumentManagerThread = new QThread(this);
    mInstrumentSourceManager->moveToThread(mInstrumentManagerThread);

    mInstrumentManagerThread->start();
    connect(ui->connectSettings->DetectSpectrumBtn, &QPushButton::clicked, this, [=]() {
        QString ip = ui->connectSettings->SpectrumResourceLineEdit->text();
        bool status = mInstrumentSourceManager->connectToN9040B(ip.toStdString());
        if (status) {
            ui->connectSettings->DetectSpectrumBtn->setText("连接成功");
        } else {
            ui->connectSettings->DetectSpectrumBtn->setText("连接失败");
        }
    });

    connect(ui->connectSettings->DetectGeneratorBtn, &QPushButton::clicked, this, [=]() {
        QString ip = ui->connectSettings->GeneratorResourceLineEdit->text();
        bool status = mInstrumentSourceManager->connectToSMA100B(ip.toStdString());
        if (status) {
            ui->connectSettings->DetectGeneratorBtn->setText("连接成功");
//            handleAutoCaliInstrument();
            mInstrumentSourceManager->sma100B->setExternalClock();
//            mInstrumentSourceManager->sma100B->SetFrequency((int)
//                        (1e6*ui->connectSettings->GeneratorResourceFreqSpinBox->value()));
        } else {
            ui->connectSettings->DetectGeneratorBtn->setText("连接失败");
        }
    });

    connect(ui->connectSettings->DetectVoltmeterBtn, &QPushButton::clicked, this, [=]() {
        QString ip = ui->connectSettings->VoltmeterResourceLineEdit->text();
        bool status = mInstrumentSourceManager->connectTo3458A(ip.toStdString());
        if (status) {
            ui->connectSettings->DetectVoltmeterBtn->setText("连接成功");
        } else {
            ui->connectSettings->DetectVoltmeterBtn->setText("连接失败");
        }
    });

    connect(ui->connectSettings->DetectGeneratorBtn2, &QPushButton::clicked, this, [=]() {
        QString ip = ui->connectSettings->GeneratorResourceLineEdit2->text();
        bool status = mInstrumentSourceManager->connectTo3362A(ip.toStdString());
        if (status) {
            ui->connectSettings->DetectGeneratorBtn2->setText("连接成功");
        } else {
            ui->connectSettings->DetectGeneratorBtn2->setText("连接失败");
        }
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
    chartWidget2 = ui->chartWidget2;
    chartWidget3 = ui->chartWidget3;
    chartWidget4 = ui->chartWidget4;

    connect(mCaculateParams, &CaculateParams::TransferFFTData, chartWidget1, &SpectrumChartWidget::handleRefreshSpectrum);
    connect(mCaculateParams, &CaculateParams::TransferPeakData, chartWidget1, &SpectrumChartWidget::handleRefreshPeakData);
    connect(ui->FindPeakButton, &QPushButton::clicked, chartWidget1, &SpectrumChartWidget::handleFindPeak);
    connect(ui->FindNextButton, &QPushButton::clicked, chartWidget1, &SpectrumChartWidget::handleFindNextPeak);

    connect(mCaculateParams, &CaculateParams::TransferADCData, chartWidget2, &SpectrumChartWidget::handleRefreshSpectrum);

    connect(mCaculateParams, &CaculateParams::TransferDNLData, chartWidget3, &SpectrumChartWidget::handleRefreshSpectrum);
    connect(mCaculateParams, &CaculateParams::TransferINLData, chartWidget4, &SpectrumChartWidget::handleRefreshSpectrum);


    connect(ui->ADCChannel1CheckBox, &QCheckBox::checkStateChanged, this, [this](int state) {
        if (state == Qt::Checked) {
            if(mDeviceType == DeviceType::AD9268){
                if(!mUdpStartFlag) {
                    QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
                    return;
                }
                QMetaObject::invokeMethod(mUdpWorker, "handleSetAD9268Channel", Qt::QueuedConnection, Q_ARG(int, 1));
            }
            ui->ADCChannel2CheckBox->setChecked(false);
        }
    });
    connect(ui->ADCChannel2CheckBox, &QCheckBox::checkStateChanged, this, [this](int state) {
        if (state == Qt::Checked) {
            if(mDeviceType == DeviceType::AD9268){
                if(!mUdpStartFlag) {
                    QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
                    return;
                }
                QMetaObject::invokeMethod(mUdpWorker, "handleSetAD9268Channel", Qt::QueuedConnection, Q_ARG(int, 2));
            }
            ui->ADCChannel1CheckBox->setChecked(false);
        }
    });


    connect(ui->DACFrequencySetButton, &QPushButton::clicked, this, [=](){
        int freq = (int)((double)ui->DACFrequencySpinBox->value() * 1e6);
        if (!mUdpStartFlag) {
            QMessageBox::warning(this, "Warning", "请先打开UDP连接！");
            return;
        }
        if(mDeviceType == DeviceType::AD9142 || mDeviceType == DeviceType::AD9747) {

            QMetaObject::invokeMethod(mUdpWorker, "handleSetDACValue", Qt::QueuedConnection, Q_ARG(int, 32767));

            QMetaObject::invokeMethod(mUdpWorker, "handleSetDDSFreq", Qt::QueuedConnection, Q_ARG(int, freq), Q_ARG(int,2));

        }else{
            QMessageBox::warning(this, "Warning", "当前不是DAC！");
            return;
        }
    });



//    chartWidget3->adjustaxisX();
    mCaculateParams->setData(adc16Data);
    chartWidget1->setSampleRate(1e8); // 设置采样率
    mCalculateThread->start();
    QMetaObject::invokeMethod(mCaculateParams, "caculateDynamicParamsADC", Qt::QueuedConnection);

    connect(ui->dynamicParamsADCTestButton, &QPushButton::clicked, this, &platform_demo_test::handleDynamicADCTest);
    connect(ui->staticParamsADCTestButton, &QPushButton::clicked, this, &platform_demo_test::handleStaticADCTest);
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
    // mUdpThread->quit();
    // mUdpThread->wait();
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
    mCalculateThread->quit();
    mCalculateThread->wait();
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

        // 调整输出幅度
        sma100b_amp += 0.1;
        mInstrumentSourceManager->sma100B->setAMP(sma100b_amp);


        emit clearADCData();
        if(mDeviceType == AD9268){
            if(maxValue > 65535*0.95){
                qDebug() << "AutoCali test suceessfully" << "Now amp is" << sma100b_amp;
                return;
            }
        }else if(mDeviceType == AD9434){
            if(maxValue > 4095*0.95){
                qDebug() << "AutoCali test suceessfully" << "Now amp is" << sma100b_amp;
                return;
            }
        }
        

//        QThread::msleep(100);

        if(sma100b_amp > 15)
            return;


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

        // todo:计算获取的数据总量是否满足置信度要求，决定是否关闭循环
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

        // todo:再获取一次数据（计算完后执行，signal&slot）
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



    // 2.获取adc数据 计算动态参数
    QString message = "Hello, World!";      // 修改为获取一次ADC数据的命令
    if (!message.isEmpty()) {
        QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
    }
    mCaculateMode = ADC_DYNAMIC_MODE;

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


    // todo: 2.获取第一次adc数据


    // todo: 3.设置标志位使计算完成后循环直至达到条件
    QString message = "Hello, World!";      // 修改为获取一次ADC数据的命令
    if (!message.isEmpty()) {
        QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
    }
    mCaculateMode = ADC_STATIC_MODE;
    adcStaticTestStop = false;
}



void platform_demo_test::handleAutoCaliInstrument() {
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
        QMessageBox::warning(this, "Warning", "当前不是ADC设备，无法进行自动校准！");
        return;
    }


    // 判断使用仪器类型


    mInstrumentType = KS3362A;
    mInstrumentSourceManager->sma100B->SetOutput1Status(false);
    mInstrumentSourceManager->sma100B->setAMP(0);
    sma100b_amp = 0;
//    QThread::msleep(100);
    mInstrumentSourceManager->sma100B->SetOutput1Status(true);  


    // 2.获取adc数据 计算动态参数
    QString message = "Hello, World!";      // 修改为获取一次ADC数据的命令
    if (!message.isEmpty()) {
        QMetaObject::invokeMethod(mUdpWorker, "sendMessage", Q_ARG(QString, message));
    }
    mCaculateMode = AUTO_CALI_MODE;

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
    }else{
        QMetaObject::invokeMethod(mUdpWorker, "handleSetAD9518InternalClock",Qt::QueuedConnection);
    }

    // todo:设置sma100b输出频率和外部时钟设置



}

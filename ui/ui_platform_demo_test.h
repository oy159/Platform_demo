#ifndef UI_PLATFORM_DEMO_TEST_H
#define UI_PLATFORM_DEMO_TEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QMenu>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QComboBox>
#include <QCheckBox>
#include <SpectrumChartWidget.h>
#include <SpectrumChartTryWidget.h>
#include <ChartWidgetsManager.h>
#include <ConnectSettings.h>

QT_BEGIN_NAMESPACE

class Ui_platform_demo_test
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QSplitter *splitter;
    QWidget *leftWidget;
    QVBoxLayout *controlLayout;


    QGroupBox *ipGroupBox;
    QVBoxLayout *ipGroupBoxLayout;
    QPushButton *connectButton;


    QGroupBox *messageGroupBox;
    QVBoxLayout *messageGroupBoxLayout;
    QPushButton *instrumentDetectBtn;
    QPushButton *instrumentSettingsBtn;


    QPushButton *switchWidgetButton;
    QPushButton *ChartWidgetControlButton;
    QSpacerItem *verticalSpacer;

    QStackedWidget *rightStackedWidget;  // 主堆叠部件
    
    // ADC参数显示部件
    QWidget *DisplayADCParamsWidget;
    QVBoxLayout *DisplayADCParamsLayout;

    QGroupBox *DynamicParamsADCGroupBox;
    QVBoxLayout *DynamicParamsADCGroupBoxLayout;
    QLabel *SFDRADCLabel;
    QLabel *SNRADCLabel;
    QLabel *THDADCLabel;
    QLabel *ENOBADCLabel;
    // QPushButton *FindPeakButton;
    // QPushButton *FindNextButton;

    QGroupBox *StaticParamsADCGroupBox;
    QVBoxLayout *StaticParamsADCGroupBoxLayout;
    QLabel *DNLADCLabel;
    QLabel *INLADCLabel;
    QLabel *OffsetADCLabel;
    QLabel *PeakADCLabel;

    QGroupBox *WindowsFuncGroupBox;
    QVBoxLayout *WindowsFuncGroupBoxLayout;
    QLabel *WindowsFuncLabel;
    QComboBox *WindowsFuncCombox;

    // ADC通道选择部件
    QGroupBox *ADCChannelChoiceGroupBox;
    QHBoxLayout *ADCChannelChoiceGroupBoxLayout;
    QLabel *ADCChannelChoiceLabel;
    QCheckBox *ADCChannel1CheckBox;
    QCheckBox *ADCChannel2CheckBox;

    // 参数测试按钮
    QGroupBox *paramTestADCGroupBox;
    QVBoxLayout *paramTestADCGroupBoxLayout;
    QPushButton *dynamicParamsADCTestButton;
    QPushButton *staticParamsADCTestButton;
    QPushButton *twoTonesADCTestButton;
    QProgressBar *staticParamsADCTestProgressBar;

    // DAC参数显示部件
    QWidget *DisplayDACParamsWidget;
    QVBoxLayout *DisplayDACParamsLayout;
    QGroupBox *DynamicParamsDACGroupBox;
    QVBoxLayout *DynamicParamsDACGroupBoxLayout;
    QLabel *SFDRDACLabel;
    QLabel *SNRDACLabel;
    QLabel *THDDACLabel;
    QLabel *ENOBDACLabel;

    QLabel *DACFrequencyLabel;
    QDoubleSpinBox *DACFrequencySpinBox;
    QPushButton *DACFrequencySetButton;

    // DAC静态参数
    QGroupBox *StaticParamsDACGroupBox;
    QVBoxLayout *StaticParamsDACGroupBoxLayout;
    QLabel *DNLDACLabel;
    QLabel *INLDACLabel;
    QLabel *OffsetDACLabel;
    QLabel *PeakDACLabel;


    // 参数测试按钮
    QGroupBox *paramTestDACGroupBox;
    QVBoxLayout *paramTestDACGroupBoxLayout;
    QPushButton *dynamicParamsDACTestButton;
    QPushButton *staticParamsDACTestButton;
    QProgressBar *staticParamsDACTestProgressBar;

    ChartWidgetsManager *chartGridWidget;
    ChartControlWidget *chartControlWidget;
    SpectrumChartTryWidget *chartWidget1;
    BaseChartWidget *chartWidget2;
    BaseChartWidget *chartWidget3;
    BaseChartWidget *chartWidget4;
    BaseChartWidget *chartWidget5;
    BaseChartWidget *chartWidget6;
    BaseChartWidget *chartWidget7; // 新增图表部件
    // QMenuBar *menubar;
    QStatusBar *statusbar;
    // QMenu *menuFile;
    // QMenu *menuSettings;
    // QAction *connectSetAction;

    ConnectSettings *connectSettings;

    void setupUi(QMainWindow *platform_demo_test)
    {
        if (platform_demo_test->objectName().isEmpty())
            platform_demo_test->setObjectName("platform_demo_test");
        platform_demo_test->resize(1400, 800);

        centralwidget = new QWidget(platform_demo_test);
        mainLayout = new QVBoxLayout(centralwidget);

        splitter = new QSplitter(centralwidget);
        splitter->setOrientation(Qt::Horizontal);

        leftWidget = new QWidget(splitter);
        controlLayout = new QVBoxLayout(leftWidget);

        ipGroupBox = new QGroupBox("设备连接测试", leftWidget);
        ipGroupBoxLayout = new QVBoxLayout(ipGroupBox);

        connectButton = new QPushButton("连接设备", ipGroupBox);
        connectButton->setCheckable(true);
        ipGroupBoxLayout->addWidget(connectButton);

        messageGroupBox = new QGroupBox("仪器资源", leftWidget);
        messageGroupBoxLayout = new QVBoxLayout(messageGroupBox);
        instrumentDetectBtn = new QPushButton("检测仪器", messageGroupBox);
        instrumentSettingsBtn = new QPushButton("仪器设置", messageGroupBox);
        messageGroupBoxLayout->addWidget(instrumentDetectBtn);
        messageGroupBoxLayout->addWidget(instrumentSettingsBtn);

        // 添加切换按钮
        switchWidgetButton = new QPushButton("切换显示部件 (ADC/DAC)", leftWidget);
        ChartWidgetControlButton = new QPushButton("图表控制", leftWidget);

        controlLayout->addWidget(ipGroupBox);
        controlLayout->addWidget(messageGroupBox);
        controlLayout->addWidget(switchWidgetButton);
        controlLayout->addWidget(ChartWidgetControlButton);


        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        controlLayout->addItem(verticalSpacer);

        leftWidget->setLayout(controlLayout);

        // 创建主堆叠部件
        rightStackedWidget = new QStackedWidget(splitter);

        // 创建并设置ADC参数显示部件
        DisplayADCParamsWidget = new QWidget();
        DisplayADCParamsLayout = new QVBoxLayout(DisplayADCParamsWidget);

        DynamicParamsADCGroupBox = new QGroupBox("ADC Dynamic Parameters", DisplayADCParamsWidget);
        DynamicParamsADCGroupBoxLayout = new QVBoxLayout(DynamicParamsADCGroupBox);
        
        SFDRADCLabel = new QLabel("SFDR: ", DynamicParamsADCGroupBox);
        SNRADCLabel = new QLabel("SNR: ", DynamicParamsADCGroupBox);
        THDADCLabel = new QLabel("THD: ", DynamicParamsADCGroupBox);
        ENOBADCLabel = new QLabel("ENOB: ", DynamicParamsADCGroupBox);
        // FindPeakButton = new QPushButton("Find Peak", DynamicParamsADCGroupBox);
        // FindNextButton = new QPushButton("Find Next", DynamicParamsADCGroupBox);
        
        DynamicParamsADCGroupBoxLayout->addWidget(SFDRADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(SNRADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(THDADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(ENOBADCLabel);
        // DynamicParamsADCGroupBoxLayout->addWidget(FindPeakButton);
        // DynamicParamsADCGroupBoxLayout->addWidget(FindNextButton);

        StaticParamsADCGroupBox = new QGroupBox("ADC Static Parameters", DisplayADCParamsWidget);
        StaticParamsADCGroupBoxLayout = new QVBoxLayout(StaticParamsADCGroupBox);

        DNLADCLabel = new QLabel("DNL: ", StaticParamsADCGroupBox);
        INLADCLabel = new QLabel("INL: ", StaticParamsADCGroupBox);
        OffsetADCLabel = new QLabel("Offset: ", StaticParamsADCGroupBox);
        PeakADCLabel = new QLabel("Peak: ", StaticParamsADCGroupBox);

        StaticParamsADCGroupBoxLayout->addWidget(DNLADCLabel);
        StaticParamsADCGroupBoxLayout->addWidget(INLADCLabel);
        StaticParamsADCGroupBoxLayout->addWidget(OffsetADCLabel);
        StaticParamsADCGroupBoxLayout->addWidget(PeakADCLabel);

        WindowsFuncGroupBox = new QGroupBox("Window Function", DisplayADCParamsWidget);
        WindowsFuncGroupBoxLayout = new QVBoxLayout(WindowsFuncGroupBox);
        WindowsFuncLabel = new QLabel("Window Function: ", WindowsFuncGroupBox);
        WindowsFuncCombox = new QComboBox(WindowsFuncGroupBox);
        WindowsFuncCombox->addItem("Hanning Window");
        WindowsFuncCombox->addItem("Hamming Window");
        WindowsFuncCombox->addItem("Rectangular Window");

        WindowsFuncGroupBoxLayout->addWidget(WindowsFuncLabel);
        WindowsFuncGroupBoxLayout->addWidget(WindowsFuncCombox);

        ADCChannelChoiceGroupBox = new QGroupBox("ADC Channel Selection", DisplayADCParamsWidget);
        ADCChannelChoiceGroupBoxLayout = new QHBoxLayout(ADCChannelChoiceGroupBox);
        ADCChannelChoiceLabel = new QLabel("通道选择: ", ADCChannelChoiceGroupBox);
        ADCChannel1CheckBox = new QCheckBox("通道1", ADCChannelChoiceGroupBox);
        ADCChannel2CheckBox = new QCheckBox("通道2", ADCChannelChoiceGroupBox);
        ADCChannel1CheckBox->setChecked(true);  // 默认选中通道1
        ADCChannel2CheckBox->setChecked(false); // 默认不选中通道2

        ADCChannelChoiceGroupBoxLayout->addWidget(ADCChannelChoiceLabel);
        ADCChannelChoiceGroupBoxLayout->addWidget(ADCChannel1CheckBox);
        ADCChannelChoiceGroupBoxLayout->addWidget(ADCChannel2CheckBox);


        paramTestADCGroupBox = new QGroupBox("参数测试", DisplayADCParamsWidget);
        paramTestADCGroupBoxLayout = new QVBoxLayout(paramTestADCGroupBox);
        dynamicParamsADCTestButton = new QPushButton("动态参数测试", paramTestADCGroupBox);
        staticParamsADCTestButton = new QPushButton("静态参数测试", paramTestADCGroupBox);
        twoTonesADCTestButton = new QPushButton("双音测试", paramTestADCGroupBox);
        staticParamsADCTestProgressBar = new QProgressBar(paramTestADCGroupBox);
        staticParamsADCTestProgressBar->setRange(0, 100);
        paramTestADCGroupBoxLayout->addWidget(dynamicParamsADCTestButton);
        paramTestADCGroupBoxLayout->addWidget(twoTonesADCTestButton);
        paramTestADCGroupBoxLayout->addWidget(staticParamsADCTestButton);
        paramTestADCGroupBoxLayout->addWidget(staticParamsADCTestProgressBar);

        
        DisplayADCParamsLayout->addWidget(DynamicParamsADCGroupBox);
        DisplayADCParamsLayout->addWidget(StaticParamsADCGroupBox);
        DisplayADCParamsLayout->addWidget(WindowsFuncGroupBox);
        DisplayADCParamsLayout->addWidget(ADCChannelChoiceGroupBox);
        DisplayADCParamsLayout->addWidget(paramTestADCGroupBox);
        DisplayADCParamsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
        DisplayADCParamsWidget->setLayout(DisplayADCParamsLayout);

        // 创建并设置DAC参数显示部件
        DisplayDACParamsWidget = new QWidget();
        DisplayDACParamsLayout = new QVBoxLayout(DisplayDACParamsWidget);

        DynamicParamsDACGroupBox = new QGroupBox("DAC Dynamic Parameters", DisplayDACParamsWidget);
        DynamicParamsDACGroupBoxLayout = new QVBoxLayout(DynamicParamsDACGroupBox);
        
        SFDRDACLabel = new QLabel("SFDR: ", DynamicParamsDACGroupBox);
        SNRDACLabel = new QLabel("SNR: ", DynamicParamsDACGroupBox);
        THDDACLabel = new QLabel("THD: ", DynamicParamsDACGroupBox);
        ENOBDACLabel = new QLabel("ENOB: ", DynamicParamsDACGroupBox);

        DACFrequencyLabel = new QLabel("DAC频率设置: ", DynamicParamsDACGroupBox);
        DACFrequencySpinBox = new QDoubleSpinBox(DynamicParamsDACGroupBox);
        DACFrequencySpinBox->setRange(0.1, 199.999); // 设置DAC频率范围为0.1MHz到100MHz
        DACFrequencySpinBox->setSingleStep(0.1); // 设置步长为0.1MHz
        DACFrequencySpinBox->setValue(10.0); // 默认值为10MHz
        DACFrequencySpinBox->setDecimals(5); // 设置小数点后保留3位
        auto* DACFrequencyUnitLabel = new QLabel("MHz", DynamicParamsDACGroupBox);

        auto* DACFrequencyLayout = new QHBoxLayout();
        DACFrequencyLayout->addWidget(DACFrequencyLabel);
        DACFrequencyLayout->addWidget(DACFrequencySpinBox);
        DACFrequencyLayout->addWidget(DACFrequencyUnitLabel);

        DACFrequencySetButton = new QPushButton("设置DAC频率", DynamicParamsDACGroupBox);

        
        DynamicParamsDACGroupBoxLayout->addWidget(SFDRDACLabel);
        DynamicParamsDACGroupBoxLayout->addWidget(SNRDACLabel);
        DynamicParamsDACGroupBoxLayout->addWidget(THDDACLabel);
        DynamicParamsDACGroupBoxLayout->addWidget(ENOBDACLabel);
        DynamicParamsDACGroupBoxLayout->addLayout(DACFrequencyLayout); // 添加DAC频率设置布局
        DynamicParamsDACGroupBoxLayout->addWidget(DACFrequencySetButton);


        StaticParamsDACGroupBox =  new QGroupBox("DAC Static Parameters", DisplayDACParamsWidget);
        StaticParamsDACGroupBoxLayout = new QVBoxLayout(StaticParamsDACGroupBox);

        DNLDACLabel = new QLabel("DNL: ", StaticParamsDACGroupBox);
        INLDACLabel = new QLabel("INL: ", StaticParamsDACGroupBox);
        OffsetDACLabel = new QLabel("Offset: ", StaticParamsDACGroupBox);
        PeakDACLabel = new QLabel("Peak: ", StaticParamsDACGroupBox);

        StaticParamsDACGroupBoxLayout->addWidget(DNLDACLabel);
        StaticParamsDACGroupBoxLayout->addWidget(INLDACLabel);
        StaticParamsDACGroupBoxLayout->addWidget(OffsetDACLabel);
        StaticParamsDACGroupBoxLayout->addWidget(PeakDACLabel);


        paramTestDACGroupBox = new QGroupBox("参数测试", DisplayDACParamsWidget);
        paramTestDACGroupBoxLayout = new QVBoxLayout(paramTestDACGroupBox);
        dynamicParamsDACTestButton = new QPushButton("动态参数测试", paramTestDACGroupBox);
        staticParamsDACTestButton = new QPushButton("静态参数测试", paramTestDACGroupBox);
        staticParamsDACTestProgressBar = new QProgressBar(paramTestADCGroupBox);
        staticParamsDACTestProgressBar->setRange(0, 100);

        paramTestDACGroupBoxLayout->addWidget(dynamicParamsDACTestButton);
        paramTestDACGroupBoxLayout->addWidget(staticParamsDACTestButton);
        paramTestDACGroupBoxLayout->addWidget(staticParamsDACTestProgressBar);

        DisplayDACParamsLayout->addWidget(DynamicParamsDACGroupBox);
        DisplayDACParamsLayout->addWidget(StaticParamsDACGroupBox);
        DisplayDACParamsLayout->addWidget(paramTestDACGroupBox);
        DisplayDACParamsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
        DisplayDACParamsWidget->setLayout(DisplayDACParamsLayout);


        // 将两个显示部件添加到堆叠部件中
        rightStackedWidget->addWidget(DisplayADCParamsWidget);
        rightStackedWidget->addWidget(DisplayDACParamsWidget);
        rightStackedWidget->setCurrentIndex(0); // 默认显示ADC部件

        chartGridWidget = new ChartWidgetsManager(splitter);
        chartWidget1 = new SpectrumChartTryWidget;
        chartWidget2 = new BaseChartWidget;
        chartWidget3 = new BaseChartWidget;
        chartWidget4 = new BaseChartWidget;
        chartWidget5 = new BaseChartWidget;
        chartWidget6 = new BaseChartWidget;
        chartWidget7 = new BaseChartWidget; 
        chartGridWidget->addChart(chartWidget1);
        chartGridWidget->addChart(chartWidget2);
        chartGridWidget->addChart(chartWidget3);
        chartGridWidget->addChart(chartWidget4);
        chartGridWidget->addChart(chartWidget5);
        chartGridWidget->addChart(chartWidget6);
        chartGridWidget->addChart(chartWidget7);

        chartGridWidget->reorganizeCharts(); // 初始化图表布局
        chartControlWidget = new ChartControlWidget();
        chartControlWidget->hide();
        chartControlWidget->setAttribute(Qt::WA_QuitOnClose, false);

        splitter->addWidget(leftWidget);
        splitter->addWidget(rightStackedWidget);
        splitter->addWidget(chartGridWidget);
        splitter->setStretchFactor(0, 15);
        splitter->setStretchFactor(1, 10);
        splitter->setStretchFactor(2, 50);

        mainLayout->addWidget(splitter);
        centralwidget->setLayout(mainLayout);

        platform_demo_test->setCentralWidget(centralwidget);

        connectSettings = new ConnectSettings(platform_demo_test);
        connectSettings->hide();
        connectSettings->setAttribute(Qt::WA_QuitOnClose, false);
        

        statusbar = new QStatusBar(platform_demo_test);
        statusbar->setObjectName("statusbar");
        platform_demo_test->setStatusBar(statusbar);

        retranslateUi(platform_demo_test);

        QMetaObject::connectSlotsByName(platform_demo_test);
    } // setupUi

    void retranslateUi(QMainWindow *platform_demo_test)
    {
        platform_demo_test->setWindowTitle(QCoreApplication::translate("platform_demo_test", "platform_demo_test", nullptr));
        // menuFile->setTitle(QCoreApplication::translate("platform_demo_test", "File", nullptr));
        // menuSettings->setTitle(QCoreApplication::translate("platform_demo_test", "Settings", nullptr));
        switchWidgetButton->setText(QCoreApplication::translate("platform_demo_test", "切换显示部件 (ADC/DAC)", nullptr));

    } // retranslateUi

};

namespace Ui {
    class platform_demo_test: public Ui_platform_demo_test {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLATFORM_DEMO_TEST_H
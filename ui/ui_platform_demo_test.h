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
#include <SpectrumChartWidget.h>
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
    QPushButton *sendButton;


    QPushButton *switchWidgetButton;  // 新增切换按钮
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
    QPushButton *FindPeakButton;
    QPushButton *FindNextButton;

    QGroupBox *StaticParamsADCGroupBox;
    QVBoxLayout *StaticParamsADCGroupBoxLayout;
    QLabel *DNLADCLabel;
    QLabel *INLADCLabel;
    QLabel *OffsetADCLabel;

    QGroupBox *WindowsFuncGroupBox;
    QVBoxLayout *WindowsFuncGroupBoxLayout;
    QLabel *WindowsFuncLabel;
    QComboBox *WindowsFuncCombox;

    // 参数测试按钮
    QGroupBox *paramTestADCGroupBox;
    QVBoxLayout *paramTestADCGroupBoxLayout;
    QPushButton *dynamicParamsADCTestButton;
    QPushButton *staticParamsADCTestButton;


    // DAC参数显示部件
    QWidget *DisplayDACParamsWidget;
    QVBoxLayout *DisplayDACParamsLayout;
    QGroupBox *DynamicParamsDACGroupBox;
    QVBoxLayout *DynamicParamsDACGroupBoxLayout;
    QLabel *SFDRDACLabel;
    QLabel *SNRDACLabel;
    QLabel *THDDACLabel;
    QLabel *ENOBDACLabel;


    // 参数测试按钮
    QGroupBox *paramTestDACGroupBox;
    QVBoxLayout *paramTestDACGroupBoxLayout;
    QPushButton *dynamicParamsDACTestButton;
    QPushButton *staticParamsDACTestButton;

    ChartWidgetsManager *chartGridWidget;  // 新增图表网格部件
    SpectrumChartWidget *chartWidget1;
    SpectrumChartWidget *chartWidget2;
    SpectrumChartWidget *chartWidget3;
    SpectrumChartWidget *chartWidget4;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QMenu *menuFile;
    QMenu *menuSettings;
    QAction *connectSetAction;

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

        messageGroupBox = new QGroupBox("仪器资源检测", leftWidget);
        messageGroupBoxLayout = new QVBoxLayout(messageGroupBox);
        sendButton = new QPushButton("检测仪器", messageGroupBox);

        messageGroupBoxLayout->addWidget(sendButton);

        // 添加参数测试按钮组
        

        // 添加切换按钮
        switchWidgetButton = new QPushButton("切换显示部件 (ADC/DAC)", leftWidget);

        connectSettings = new ConnectSettings(leftWidget);
        
        controlLayout->addWidget(ipGroupBox);
        controlLayout->addWidget(messageGroupBox);
        // controlLayout->addWidget(paramTestADCGroupBox);  // 添加参数测试按钮组
        controlLayout->addWidget(switchWidgetButton);  // 添加切换按钮
        controlLayout->addWidget(connectSettings);  // 添加连接设置部件

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
        FindPeakButton = new QPushButton("Find Peak", DynamicParamsADCGroupBox);
        FindNextButton = new QPushButton("Find Next", DynamicParamsADCGroupBox);
        
        DynamicParamsADCGroupBoxLayout->addWidget(SFDRADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(SNRADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(THDADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(ENOBADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(FindPeakButton);
        DynamicParamsADCGroupBoxLayout->addWidget(FindNextButton);

        StaticParamsADCGroupBox = new QGroupBox("ADC Static Parameters", DisplayADCParamsWidget);
        StaticParamsADCGroupBoxLayout = new QVBoxLayout(StaticParamsADCGroupBox);

        DNLADCLabel = new QLabel("DNL: ", StaticParamsADCGroupBox);
        INLADCLabel = new QLabel("INL: ", StaticParamsADCGroupBox);
        OffsetADCLabel = new QLabel("Offset: ", StaticParamsADCGroupBox);

        StaticParamsADCGroupBoxLayout->addWidget(DNLADCLabel);
        StaticParamsADCGroupBoxLayout->addWidget(INLADCLabel);
        StaticParamsADCGroupBoxLayout->addWidget(OffsetADCLabel);


        WindowsFuncGroupBox = new QGroupBox("Window Function", DisplayADCParamsWidget);
        WindowsFuncGroupBoxLayout = new QVBoxLayout(WindowsFuncGroupBox);
        WindowsFuncLabel = new QLabel("Window Function: ", WindowsFuncGroupBox);
        WindowsFuncCombox = new QComboBox(WindowsFuncGroupBox);
        WindowsFuncCombox->addItem("Hanning Window");
        WindowsFuncCombox->addItem("Hamming Window");

        WindowsFuncGroupBoxLayout->addWidget(WindowsFuncLabel);
        WindowsFuncGroupBoxLayout->addWidget(WindowsFuncCombox);


        paramTestADCGroupBox = new QGroupBox("参数测试", DisplayADCParamsWidget);
        paramTestADCGroupBoxLayout = new QVBoxLayout(paramTestADCGroupBox);
        dynamicParamsADCTestButton = new QPushButton("动态参数测试", paramTestADCGroupBox);
        staticParamsADCTestButton = new QPushButton("静态参数测试", paramTestADCGroupBox);
        paramTestADCGroupBoxLayout->addWidget(dynamicParamsADCTestButton);
        paramTestADCGroupBoxLayout->addWidget(staticParamsADCTestButton);

        
        DisplayADCParamsLayout->addWidget(DynamicParamsADCGroupBox);
        DisplayADCParamsLayout->addWidget(StaticParamsADCGroupBox);
        DisplayADCParamsLayout->addWidget(WindowsFuncGroupBox);
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
        
        DynamicParamsDACGroupBoxLayout->addWidget(SFDRDACLabel);
        DynamicParamsDACGroupBoxLayout->addWidget(SNRDACLabel);
        DynamicParamsDACGroupBoxLayout->addWidget(THDDACLabel);
        DynamicParamsDACGroupBoxLayout->addWidget(ENOBDACLabel);


        paramTestDACGroupBox = new QGroupBox("参数测试", DisplayDACParamsWidget);
        paramTestDACGroupBoxLayout = new QVBoxLayout(paramTestDACGroupBox);
        dynamicParamsDACTestButton = new QPushButton("动态参数测试", paramTestDACGroupBox);
        staticParamsDACTestButton = new QPushButton("静态参数测试", paramTestDACGroupBox);

        paramTestDACGroupBoxLayout->addWidget(dynamicParamsDACTestButton);
        paramTestDACGroupBoxLayout->addWidget(staticParamsDACTestButton);

        DisplayDACParamsLayout->addWidget(DynamicParamsDACGroupBox);
        DisplayDACParamsLayout->addWidget(paramTestDACGroupBox);
        DisplayDACParamsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
        DisplayDACParamsWidget->setLayout(DisplayDACParamsLayout);


        // 将两个显示部件添加到堆叠部件中
        rightStackedWidget->addWidget(DisplayADCParamsWidget);
        rightStackedWidget->addWidget(DisplayDACParamsWidget);
        rightStackedWidget->setCurrentIndex(0); // 默认显示ADC部件

        chartGridWidget = new ChartWidgetsManager(splitter);
        chartWidget1 = new SpectrumChartWidget;
        chartWidget2 = new SpectrumChartWidget; // 可以添加更多图表
        chartWidget3 = new SpectrumChartWidget;
        chartWidget4 = new SpectrumChartWidget;
        chartGridWidget->addChart(chartWidget1);
        chartGridWidget->addChart(chartWidget2);
        chartGridWidget->addChart(chartWidget3);
        chartGridWidget->addChart(chartWidget4);

        chartGridWidget->reorganizeCharts(); // 初始化图表布局

        splitter->addWidget(leftWidget);
        splitter->addWidget(rightStackedWidget);  // 使用堆叠部件替换原来的DisplayADCParamsWidget
        splitter->addWidget(chartGridWidget);
        splitter->setStretchFactor(0, 30);
        splitter->setStretchFactor(1, 1);
        splitter->setStretchFactor(2, 500);

        mainLayout->addWidget(splitter);
        centralwidget->setLayout(mainLayout);

        platform_demo_test->setCentralWidget(centralwidget);

        menubar = new QMenuBar(platform_demo_test);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        platform_demo_test->setMenuBar(menubar);

        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menubar->addAction(menuFile->menuAction());

        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName("menuSettings");
        menubar->addAction(menuSettings->menuAction());

        connectSetAction = menuSettings->addAction("Connect");

        statusbar = new QStatusBar(platform_demo_test);
        statusbar->setObjectName("statusbar");
        platform_demo_test->setStatusBar(statusbar);

        retranslateUi(platform_demo_test);

        QMetaObject::connectSlotsByName(platform_demo_test);
    } // setupUi

    void retranslateUi(QMainWindow *platform_demo_test)
    {
        platform_demo_test->setWindowTitle(QCoreApplication::translate("platform_demo_test", "platform_demo_test", nullptr));
        menuFile->setTitle(QCoreApplication::translate("platform_demo_test", "File", nullptr));
        menuSettings->setTitle(QCoreApplication::translate("platform_demo_test", "Settings", nullptr));
        switchWidgetButton->setText(QCoreApplication::translate("platform_demo_test", "切换显示部件 (ADC/DAC)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class platform_demo_test: public Ui_platform_demo_test {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLATFORM_DEMO_TEST_H
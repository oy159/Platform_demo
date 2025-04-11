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
#include <ChartWidget.h>
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
    QHBoxLayout *ipLayout;
    QLabel *ipLabel;
    QLineEdit *ipLineEdit;
    QHBoxLayout *portLayout;
    QLabel *portLabel;
    QLineEdit *portLineEdit;
    QPushButton *connectButton;
    QGroupBox *messageGroupBox;
    QVBoxLayout *messageGroupBoxLayout;
    QLabel *messageLabel;
    QLineEdit *messageLineEdit;
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

    // DAC参数显示部件
    QWidget *DisplayDACParamsWidget;
    QVBoxLayout *DisplayDACParamsLayout;
    QGroupBox *DynamicParamsDACGroupBox;
    QVBoxLayout *DynamicParamsDACGroupBoxLayout;
    QLabel *SFDRDACLabel;
    QLabel *SNRDACLabel;
    QLabel *THDDACLabel;
    QLabel *ENOBDACLabel;

    ChartWidget *chartWidget1;
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

        ipGroupBox = new QGroupBox("IP and Port Settings", leftWidget);
        ipGroupBoxLayout = new QVBoxLayout(ipGroupBox);

        ipLayout = new QHBoxLayout();
        ipLabel = new QLabel("IP Address:", ipGroupBox);
        ipLineEdit = new QLineEdit(ipGroupBox);
        ipLineEdit->setText("192.168.1.10");
        ipLayout->addWidget(ipLabel);
        ipLayout->addWidget(ipLineEdit);

        portLayout = new QHBoxLayout();
        portLabel = new QLabel("Port:", ipGroupBox);
        portLineEdit = new QLineEdit(ipGroupBox);
        portLineEdit->setText("12345");
        portLayout->addWidget(portLabel);
        portLayout->addWidget(portLineEdit);

        ipGroupBoxLayout->addLayout(ipLayout);
        ipGroupBoxLayout->addLayout(portLayout);

        connectButton = new QPushButton("点击连接设备", ipGroupBox);
        connectButton->setCheckable(true);
        ipGroupBoxLayout->addWidget(connectButton);

        messageGroupBox = new QGroupBox("Message Settings", leftWidget);
        messageGroupBoxLayout = new QVBoxLayout(messageGroupBox);
        messageLabel = new QLabel("Message:", messageGroupBox);
        messageLineEdit = new QLineEdit(messageGroupBox);
        sendButton = new QPushButton("Send", messageGroupBox);

        messageGroupBoxLayout->addWidget(messageLabel);
        messageGroupBoxLayout->addWidget(messageLineEdit);
        messageGroupBoxLayout->addWidget(sendButton);

        // 添加切换按钮
        switchWidgetButton = new QPushButton("切换显示部件 (ADC/DAC)", leftWidget);
        
        controlLayout->addWidget(ipGroupBox);
        controlLayout->addWidget(messageGroupBox);
        controlLayout->addWidget(switchWidgetButton);  // 添加切换按钮

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
        
        DynamicParamsADCGroupBoxLayout->addWidget(SFDRADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(SNRADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(THDADCLabel);
        DynamicParamsADCGroupBoxLayout->addWidget(ENOBADCLabel);
        
        DisplayADCParamsLayout->addWidget(DynamicParamsADCGroupBox);
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
        
        DisplayDACParamsLayout->addWidget(DynamicParamsDACGroupBox);
        DisplayDACParamsLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
        DisplayDACParamsWidget->setLayout(DisplayDACParamsLayout);

        // 将两个显示部件添加到堆叠部件中
        rightStackedWidget->addWidget(DisplayADCParamsWidget);
        rightStackedWidget->addWidget(DisplayDACParamsWidget);
        rightStackedWidget->setCurrentIndex(0); // 默认显示ADC部件

        chartWidget1 = new ChartWidget(splitter);

        splitter->addWidget(leftWidget);
        splitter->addWidget(rightStackedWidget);  // 使用堆叠部件替换原来的DisplayADCParamsWidget
        splitter->addWidget(chartWidget1);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 2);
        splitter->setStretchFactor(2, 5);

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
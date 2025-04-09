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
#include <ChartWidget.h>

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
    QSpacerItem *verticalSpacer;

    


    QWidget *DisplayParamsWidget;
    QVBoxLayout *DisplayParamsLayout;
    QGroupBox *DynamicParamsGroupBox;
    QVBoxLayout *DynamicParamsGroupBoxLayout;
    QLabel *SFDRLabel;
    QLabel *SNRLabel;
    QLabel *THDLabel;
    QLabel *ENOBLabel;


    ChartWidget *chartWidget1;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QMenu *menuSettings;
    QAction *ClickChart1;

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
//        ipLineEdit->setPlaceholderText("192.168.1.10");
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

        controlLayout->addWidget(ipGroupBox);
        controlLayout->addWidget(messageGroupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        controlLayout->addItem(verticalSpacer);

        leftWidget->setLayout(controlLayout);

        DisplayParamsWidget = new QWidget(splitter);
        DisplayParamsLayout = new QVBoxLayout(DisplayParamsWidget);

        DynamicParamsGroupBox = new QGroupBox("Dynamic Parameters", DisplayParamsWidget);
        DynamicParamsGroupBoxLayout = new QVBoxLayout(DynamicParamsGroupBox);
        DisplayParamsLayout->addWidget(DynamicParamsGroupBox);

        SFDRLabel = new QLabel("SFDR: ", DynamicParamsGroupBox);
        SNRLabel = new QLabel("SNR: ", DynamicParamsGroupBox);
        THDLabel = new QLabel("THD: ", DynamicParamsGroupBox);
        ENOBLabel = new QLabel("ENOB: ", DynamicParamsGroupBox);
        DynamicParamsGroupBoxLayout->addWidget(SFDRLabel);
        DynamicParamsGroupBoxLayout->addWidget(SNRLabel);
        DynamicParamsGroupBoxLayout->addWidget(THDLabel);
        DynamicParamsGroupBoxLayout->addWidget(ENOBLabel);

        DisplayParamsLayout->addWidget(DynamicParamsGroupBox);

        controlLayout->addWidget(DisplayParamsWidget);

        auto* VerticalSpacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        DisplayParamsLayout->addItem(VerticalSpacer2);

        DisplayParamsWidget->setLayout(DisplayParamsLayout);

        chartWidget1 = new ChartWidget(splitter);

        splitter->addWidget(leftWidget);
        splitter->addWidget(DisplayParamsWidget);
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

        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName("menuSettings");
        menubar->addAction(menuSettings->menuAction());
        // add a choice to the menu
        ClickChart1 = menuSettings->addAction("Charts");

        statusbar = new QStatusBar(platform_demo_test);
        statusbar->setObjectName("statusbar");
        platform_demo_test->setStatusBar(statusbar);

        retranslateUi(platform_demo_test);

        QMetaObject::connectSlotsByName(platform_demo_test);
    } // setupUi

    void retranslateUi(QMainWindow *platform_demo_test)
    {
        platform_demo_test->setWindowTitle(QCoreApplication::translate("platform_demo_test", "platform_demo_test", nullptr));
        menuSettings->setTitle(QCoreApplication::translate("platform_demo_test", "Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class platform_demo_test: public Ui_platform_demo_test {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLATFORM_DEMO_TEST_H
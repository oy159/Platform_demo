//
// Created by oycr on 2025/4/9.
//

#include <QGroupBox>
#include <QPushButton>
#include "ConnectSettings.h"

ConnectSettings::ConnectSettings(QWidget *parent) {
    resize(200, 500);

    setWindowTitle("Connect Settings");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);

    auto *ipGroupBox = new QGroupBox("控制器IP", this);
    auto *ipGroupBoxLayout = new QVBoxLayout(ipGroupBox);
    auto *ipLayout = new QHBoxLayout();
    auto *portLayout = new QHBoxLayout();

    auto *ipLabel = new QLabel("控制器IP:", ipGroupBox);
    ipLineEdit = new QLineEdit(ipGroupBox);
    ipLineEdit->setText("192.168.1.10");
    ipLineEdit->setReadOnly(true);
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(ipLineEdit);

    auto *portLabel = new QLabel("控制器端口:", ipGroupBox);
    portLineEdit = new QLineEdit(ipGroupBox);
    portLineEdit->setText("12345");
    portLineEdit->setReadOnly(true);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portLineEdit);

    ipGroupBoxLayout->addLayout(ipLayout);
    ipGroupBoxLayout->addLayout(portLayout);


    auto *SpectrumResourceGroupBox = new QGroupBox("频谱仪", this);
    auto *SpectrumResourceGroupBoxLayout = new QVBoxLayout(SpectrumResourceGroupBox);
    auto *SpectrumResourceLayout = new QVBoxLayout();

    auto *SpectrumResourceLabel = new QLabel("频谱仪资源:", SpectrumResourceGroupBox);
    SpectrumResourceLineEdit = new QLineEdit(SpectrumResourceGroupBox);
//    SpectrumResourceLineEdit->setText("TCPIP0::K-N9040B-70101.local::hislip0::INSTR");
    SpectrumResourceLineEdit->setText("TCPIP::192.168.1.4::INSTR");

    DetectSpectrumBtn = new QPushButton("Detect", SpectrumResourceGroupBox);

    SpectrumResourceLayout->addWidget(SpectrumResourceLabel);
    SpectrumResourceLayout->addWidget(SpectrumResourceLineEdit);
    SpectrumResourceLayout->addWidget(DetectSpectrumBtn);

    SpectrumResourceGroupBoxLayout->addLayout(SpectrumResourceLayout);


    auto *GeneratorResourceGroupBox = new QGroupBox("信号发生器", this);
    auto *GeneratorResourceGroupBoxLayout = new QVBoxLayout(GeneratorResourceGroupBox);
    auto *GeneratorResourceLayout = new QVBoxLayout();

    auto *GeneratorResourceLabel = new QLabel("信号发生器资源:", GeneratorResourceGroupBox);
    GeneratorResourceLineEdit = new QLineEdit(GeneratorResourceGroupBox);
    GeneratorResourceLineEdit->setText("TCPIP::192.168.1.7::hislip0::INSTR");
    DetectGeneratorBtn = new QPushButton("Detect", GeneratorResourceGroupBox);

    auto *GeneratorResourceFreqHBoxLayout = new QHBoxLayout();
    auto *GeneratorResourceFreqLabel = new QLabel("频率设置:", GeneratorResourceGroupBox);
    GeneratorResourceFreqSpinBox = new QDoubleSpinBox(GeneratorResourceGroupBox);
    GeneratorResourceFreqSpinBox->setDecimals(3);
    GeneratorResourceFreqSpinBox->setValue(10.0);
    auto *GeneratorResourceFreqUnitLabel = new QLabel("MHz", GeneratorResourceGroupBox);
    auto *QSpacerItem1 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);


    GeneratorResourceFreqHBoxLayout->addWidget(GeneratorResourceFreqLabel);
    GeneratorResourceFreqHBoxLayout->addWidget(GeneratorResourceFreqSpinBox);
    GeneratorResourceFreqHBoxLayout->addWidget(GeneratorResourceFreqUnitLabel);
    GeneratorResourceFreqHBoxLayout->addItem(QSpacerItem1);

    auto *GeneratorResourceExternalClockFreqHBoxLayout = new QHBoxLayout();
    auto *GeneratorResourceExternalClockFreqLabel = new QLabel("外部时钟频率设置:", GeneratorResourceGroupBox);
    GeneratorResourceExternalClockFreqSpinBox = new QDoubleSpinBox(GeneratorResourceGroupBox);
    GeneratorResourceExternalClockFreqSpinBox->setDecimals(3);
    GeneratorResourceExternalClockFreqSpinBox->setRange(0.001, 200.0);
    GeneratorResourceExternalClockFreqSpinBox->setValue(100.0);
     qDebug() << "Current value:" << GeneratorResourceExternalClockFreqSpinBox->value();
    auto *GeneratorResourceExternalClockFreqUnitLabel = new QLabel("MHz", GeneratorResourceGroupBox);
    auto *QSpacerItem2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    GeneratorResourceExternalClockFreqCheckBox = new QCheckBox("外部时钟",GeneratorResourceGroupBox);

    GeneratorResourceExternalClockFreqHBoxLayout->addWidget(GeneratorResourceExternalClockFreqLabel);
    GeneratorResourceExternalClockFreqHBoxLayout->addWidget(GeneratorResourceExternalClockFreqSpinBox);
    GeneratorResourceExternalClockFreqHBoxLayout->addWidget(GeneratorResourceExternalClockFreqUnitLabel);
    GeneratorResourceExternalClockFreqHBoxLayout->addItem(QSpacerItem2);
    GeneratorResourceExternalClockFreqHBoxLayout->addWidget(GeneratorResourceExternalClockFreqCheckBox);

    SettingGeneratorResourceBtn = new QPushButton("设置", GeneratorResourceGroupBox);

    GeneratorResourceLayout->addWidget(GeneratorResourceLabel);
    GeneratorResourceLayout->addWidget(GeneratorResourceLineEdit);
    GeneratorResourceLayout->addLayout(GeneratorResourceFreqHBoxLayout);
    GeneratorResourceLayout->addLayout(GeneratorResourceExternalClockFreqHBoxLayout);
    GeneratorResourceLayout->addWidget(DetectGeneratorBtn);
    GeneratorResourceLayout->addWidget(SettingGeneratorResourceBtn);

    GeneratorResourceGroupBoxLayout->addLayout(GeneratorResourceLayout);


    auto *GeneratorResourceGroupBox2 = new QGroupBox("低频信号发生器", this);
    auto *GeneratorResourceGroupBoxLayout2 = new QVBoxLayout(GeneratorResourceGroupBox2);
    auto *GeneratorResourceLayout2 = new QVBoxLayout();

    auto *GeneratorResourceLabel2 = new QLabel("低频信号发生器资源:", GeneratorResourceGroupBox2);
    GeneratorResourceLineEdit2 = new QLineEdit(GeneratorResourceGroupBox2);
    GeneratorResourceLineEdit2->setText("TCPIP0::A-33600-00000.local::inst0::INSTR");

    auto *GeneratorResourceGroupBox2Layout = new QHBoxLayout();
    auto *GeneratorResource2FreqLabel = new QLabel("频率设置:", GeneratorResourceGroupBox);
    GeneratorResource2FreqSpinBox = new QDoubleSpinBox(GeneratorResourceGroupBox);
    GeneratorResource2FreqSpinBox->setDecimals(6);
    GeneratorResource2FreqSpinBox->setValue(10.0);
    auto *GeneratorResource2FreqUnitLabel = new QLabel("MHz", GeneratorResourceGroupBox);
    auto *QSpacerItem3 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    GeneratorResourceGroupBox2Layout->addWidget(GeneratorResource2FreqLabel);
    GeneratorResourceGroupBox2Layout->addWidget(GeneratorResource2FreqSpinBox);
    GeneratorResourceGroupBox2Layout->addWidget(GeneratorResource2FreqUnitLabel);
    GeneratorResourceGroupBox2Layout->addItem(QSpacerItem3);

    
    DetectGeneratorBtn2 = new QPushButton("检测", GeneratorResourceGroupBox2);
    SettingGeneratorResourceBtn2 = new QPushButton("设置", GeneratorResourceGroupBox2);

    GeneratorResourceLayout2->addWidget(GeneratorResourceLabel2);
    GeneratorResourceLayout2->addWidget(GeneratorResourceLineEdit2);
    GeneratorResourceLayout2->addLayout(GeneratorResourceGroupBox2Layout);
    GeneratorResourceLayout2->addWidget(DetectGeneratorBtn2);
    GeneratorResourceLayout2->addWidget(SettingGeneratorResourceBtn2);

    GeneratorResourceGroupBoxLayout2->addLayout(GeneratorResourceLayout2);


    auto *VoltmeterGroupBox = new QGroupBox("台表", this);
    auto *VoltmeterGroupBoxLayout = new QVBoxLayout(VoltmeterGroupBox);
    auto *VoltmeterLayout = new QVBoxLayout();

    auto *VoltmeterLabel = new QLabel("台表资源:", VoltmeterGroupBox);
    VoltmeterResourceLineEdit = new QLineEdit(VoltmeterGroupBox);
    VoltmeterResourceLineEdit->setText("USB0::0x2A8D::0x0501::MY60048805::INSTR");
    DetectVoltmeterBtn = new QPushButton("Detect", VoltmeterGroupBox);

    VoltmeterLayout->addWidget(VoltmeterLabel);
    VoltmeterLayout->addWidget(VoltmeterResourceLineEdit);
    VoltmeterLayout->addWidget(DetectVoltmeterBtn);

    VoltmeterGroupBoxLayout->addLayout(VoltmeterLayout);


    mainLayout->addWidget(ipGroupBox);
    mainLayout->addWidget(SpectrumResourceGroupBox);
    mainLayout->addWidget(GeneratorResourceGroupBox);
    mainLayout->addWidget(GeneratorResourceGroupBox2);
    mainLayout->addWidget(VoltmeterGroupBox);

    auto *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(verticalSpacer);


}

ConnectSettings::~ConnectSettings() {
    delete ipLineEdit;
    delete portLineEdit;
    delete SpectrumResourceLineEdit;
    delete GeneratorResourceLineEdit;
    delete DetectSpectrumBtn;
    delete DetectGeneratorBtn;
}

//
// Created by oycr on 2025/4/9.
//

#include <QGroupBox>
#include <QPushButton>
#include "ConnectSettings.h"

ConnectSettings::ConnectSettings(QWidget *parent) {
    resize(300, 400);

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
    SpectrumResourceLineEdit->setText("TCPIP0::K-N9040B-70101.local::hislip0::INSTR");
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
    GeneratorResourceLineEdit->setText("TCPIP::192.168.1.5::hislip0::INSTR");
    DetectGeneratorBtn = new QPushButton("Detect", GeneratorResourceGroupBox);

    GeneratorResourceLayout->addWidget(GeneratorResourceLabel);
    GeneratorResourceLayout->addWidget(GeneratorResourceLineEdit);
    GeneratorResourceLayout->addWidget(DetectGeneratorBtn);

    GeneratorResourceGroupBoxLayout->addLayout(GeneratorResourceLayout);


    auto *GeneratorResourceGroupBox2 = new QGroupBox("低频信号发生器", this);
    auto *GeneratorResourceGroupBoxLayout2 = new QVBoxLayout(GeneratorResourceGroupBox2);
    auto *GeneratorResourceLayout2 = new QVBoxLayout();

    auto *GeneratorResourceLabel2 = new QLabel("低频信号发生器资源:", GeneratorResourceGroupBox2);
    GeneratorResourceLineEdit2 = new QLineEdit(GeneratorResourceGroupBox2);
    DetectGeneratorBtn2 = new QPushButton("Detect", GeneratorResourceGroupBox2);

    GeneratorResourceLayout2->addWidget(GeneratorResourceLabel2);
    GeneratorResourceLayout2->addWidget(GeneratorResourceLineEdit2);
    GeneratorResourceLayout2->addWidget(DetectGeneratorBtn2);

    GeneratorResourceGroupBoxLayout2->addLayout(GeneratorResourceLayout2);


    auto *VoltmeterGroupBox = new QGroupBox("台表", this);
    auto *VoltmeterGroupBoxLayout = new QVBoxLayout(VoltmeterGroupBox);
    auto *VoltmeterLayout = new QVBoxLayout();

    auto *VoltmeterLabel = new QLabel("台表资源:", VoltmeterGroupBox);
    VoltmeterResourceLineEdit = new QLineEdit(VoltmeterGroupBox);
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

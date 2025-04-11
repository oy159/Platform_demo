//
// Created by oycr on 2025/4/9.
//

#include <QGroupBox>
#include <QPushButton>
#include "ConnectSettings.h"

ConnectSettings::ConnectSettings(QWidget *parent) {
    resize(200, 300);

    setWindowTitle("Connect Settings");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);

    auto *ipGroupBox = new QGroupBox("IP Settings", this);
    auto *ipGroupBoxLayout = new QVBoxLayout(ipGroupBox);
    auto *ipLayout = new QHBoxLayout();
    auto *portLayout = new QHBoxLayout();

    auto *ipLabel = new QLabel("Target IP:", ipGroupBox);
    ipLineEdit = new QLineEdit(ipGroupBox);
    ipLineEdit->setText("192.168.1.10");
    ipLineEdit->setReadOnly(true);
    ipLayout->addWidget(ipLabel);
    ipLayout->addWidget(ipLineEdit);

    auto *portLabel = new QLabel("Remote Port:", ipGroupBox);
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


    mainLayout->addWidget(ipGroupBox);
    mainLayout->addWidget(SpectrumResourceGroupBox);
    mainLayout->addWidget(GeneratorResourceGroupBox);

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

//
// Created by oycr on 2025/4/9.
//

#ifndef PLATFORM_DEMO_CONNECTSETTINGS_H
#define PLATFORM_DEMO_CONNECTSETTINGS_H

#include "QWidget"
#include "QLineEdit"
#include <string>
#include <QLabel>
#include <QVBoxLayout>

using namespace std;

class ConnectSettings : public QWidget{
    Q_OBJECT
public:
    explicit ConnectSettings(QWidget *parent = nullptr);
    ~ConnectSettings() override;

signals:
    void    sendResourceString(const QString &SpectrumResourceString,
                    const QString &GeneratorResourceString);
    void    sendConnectSettings(const QString &target_ip,
                    const uint16_t remote_port);

public:
    QString     target_ip;
    uint16_t    remote_port;

    string      SpectrumResourceString;
    string      GeneratorResourceString;

    QLineEdit   *ipLineEdit;
    QLineEdit   *portLineEdit;
    QLineEdit   *SpectrumResourceLineEdit;
    QLineEdit   *GeneratorResourceLineEdit;
    QLineEdit   *GeneratorResourceLineEdit2;
    QLineEdit   *VoltmeterResourceLineEdit;
    

    QPushButton *DetectSpectrumBtn;
    QPushButton *DetectGeneratorBtn;
    QPushButton *DetectGeneratorBtn2;
    QPushButton *DetectVoltmeterBtn;

};


#endif //PLATFORM_DEMO_CONNECTSETTINGS_H

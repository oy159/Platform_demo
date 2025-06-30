#ifndef KSVISA_33622A_H
#define KSVISA_33622A_H

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <visa.h>
#include <stdexcept>
#include <QDebug>



class KeySightVisa_33622A {
public:
    KeySightVisa_33622A();
    ~KeySightVisa_33622A();

    bool connect(const std::string& resourceString);
    void disconnect();
    std::string sendCommand(const std::string& command);
    void sendCommandWrite(const std::string& command);

    std::string getID();
    void setFrequency(int source, double freq);
    std::string getFrequency(int source);
    void setFunc(int source, std::string func);
    std::string getFunc(int source, std::string func);
    void setOutputStatus(int channel, bool on);

    void setVoltage(int source, double Voltage);
    void setOffsetVoltage(int source, double Voltage);
    void setHighVoltage(int source, double Voltage);
    void setLowVoltage(int source, double Voltage);
    

private:
    ViSession m_defaultRM;
    ViSession m_session;

    std::string FreqToScientific(double freq) {
        std::ostringstream oss;
        oss << std::showpos << std::uppercase << std::scientific << std::setprecision(8) << freq;
        return oss.str();
    }

    std::string AmplitudeToShowpos(double amp) {
        std::ostringstream oss;
        oss << std::showpos << std::fixed << std::setprecision(4) << amp;
        return oss.str();
    }
};









#endif
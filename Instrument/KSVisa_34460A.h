#ifndef KSVISA_34460A_H
#define KSVISA_34460A_H


#include <string>
#include <vector>
#include <stdexcept>
#include <visa.h>
#include <memory>
#include <QPointF>
#include <QDebug>
#include <iomanip>
#include <sstream>


class KSVisa_34460A
{

public:
    KSVisa_34460A();
    ~KSVisa_34460A();

    bool connect(const std::string& resourceString);
    void disconnect();
    std::string sendCommand(const std::string& command);
    void sendCommandWrite(const std::string& command);

    std::string getID();
    double readVoltage();
    double readDM3068Voltage();
    void setNPLC(double NPLC);
    void setNPLCTime(double time);

private:
    ViSession m_defaultRM;
    ViSession m_session;

    std::string TimeToScientific(double time) {
        std::ostringstream oss;
        oss << std::showpos << std::uppercase << std::scientific << std::setprecision(5) << time;
        return oss.str();
    }
};


#endif
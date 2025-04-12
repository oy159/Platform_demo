#ifndef PLATFORM_DEMO_RHODESCHWARZVISA_SMA100B_H
#define PLATFORM_DEMO_RHODESCHWARZVISA_SMA100B_H

#include <string>
#include <visa.h>
#include <stdexcept>


class RhodeSchwarzVisa_SMA100B {
public:
    RhodeSchwarzVisa_SMA100B();
    ~RhodeSchwarzVisa_SMA100B();

    void connect(const std::string& resourceString);
    void disconnect();
    std::string sendCommand(const std::string& command);
    void sendCommandWrite(const std::string& command);

    std::string getID();
    void SetFrequency(int freq);
    void setAMP(double amp_dbm);
    void SetOutput1Status(bool on);

private:
    ViSession m_defaultRM;
    ViSession m_session;
};

#endif // PLATFORM_DEMO_RHODESCHWARZVISA_SMA100B_H
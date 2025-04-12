#ifndef PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H
#define PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H

#include <string>
#include <vector>
#include <stdexcept>
#include <visa.h>
#include <memory>

class KeySightVisa_N9040B {
public:
    KeySightVisa_N9040B();
    ~KeySightVisa_N9040B();

    void connect(const std::string& resourceString);
    void disconnect();
    void sendCommandWrite(const std::string& command) const;
    std::string sendCommandQuery(const std::string& command) const;

    std::string getID();
    void defineStartFreq(int freq);
    void defineStopFreq(int freq);
    void defineRefLevel(double RefLeveldbm);
    std::string readX();
    void peakSearch();

private:
    ViSession m_defaultRM;
    ViSession m_session;
};

#endif //PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H
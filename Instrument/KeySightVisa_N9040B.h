#ifndef PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H
#define PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H

#include <string>
#include <vector>
#include <stdexcept>
#include <visa.h>
#include <memory>
#include <QPointF>
#include <sstream>
#include <iomanip>

#define USE_RSA3030N

typedef enum{
    PeakSearch = 0,
    NextSearch = 1,
    RightSearch = 2,
    LeftSearch = 3,
} PeakSearchMode;


class KeySightVisa_N9040B {
public:
    KeySightVisa_N9040B();
    ~KeySightVisa_N9040B();

    bool connect(const std::string& resourceString);
    void disconnect();
    void sendCommandWrite(const std::string& command) const;
    std::string sendCommandQuery(const std::string& command) const;

    std::string getID();
    void defineStartFreq(int freq);
    void defineStopFreq(int freq);
    void defineRefLevel(double RefLeveldbm);
    void defineRBW(int freq);
    void defineVBW(int freq);
    double readMarker1Freq();
    double readMarker1Amp();
    void defineRFAttenuation(int dbm);
    void defineContinuous(bool on);

    void setMarker1X(int hz);

    void peakSearch(PeakSearchMode mode = PeakSearch);
    QVector<QPointF> readSA();

    ViSession m_defaultRM;
    ViSession m_session;

private:
    std::string readMarker1X();
    std::string readMarker1Y();
    QVector<QPointF> convertSpectrumDataToQPoints(const char* buffer, int bufferSize);
    double ScientificToFreq(const std::string& str) {
        std::istringstream iss(str);
        double freq;
        iss >> std::scientific >> freq;
        return freq;
    }
    
};

#endif //PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H
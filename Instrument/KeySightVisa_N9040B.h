#ifndef PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H
#define PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H

#include <string>
#include <vector>
#include <stdexcept>
#include <visa.h>
#include <memory>
#include <QPointF>

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
    std::string readX();
    void peakSearch(PeakSearchMode mode = PeakSearch);
    QVector<QPointF> readSA();

    ViSession m_defaultRM;
    ViSession m_session;

private:
    QVector<QPointF> convertSpectrumDataToQPoints(const char* buffer, int bufferSize);
    
};

#endif //PLATFORM_DEMO_KEYSIGHTVISA_N9040B_H
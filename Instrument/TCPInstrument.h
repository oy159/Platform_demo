#ifndef PLATFORM_DEMO_TCPINSTRUMENT_H
#define PLATFORM_DEMO_TCPINSTRUMENT_H

#include <string>
#include <stdexcept>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")  // 链接 WinSock 库

class TCPInstrument {
public:
    TCPInstrument();
    ~TCPInstrument();

    bool connect(const std::string& ip, int port = 5025);  // 默认端口 5025（SCPI 标准）
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
    SOCKET m_socket;
    bool m_connected;
};

#endif //PLATFORM_DEMO_TCPINSTRUMENT_H
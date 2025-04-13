#include "TCPInstrument.h"
#include <QDebug>
#include <thread>  // 用于延时

TCPInstrument::TCPInstrument() : m_socket(INVALID_SOCKET), m_connected(false) {
    // 初始化 WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize WinSock");
    }
}

TCPInstrument::~TCPInstrument() {
    disconnect();
    WSACleanup();  // 清理 WinSock
}

bool TCPInstrument::connect(const std::string& ip, int port) {
    if (m_connected) {
        disconnect();
    }

    // 创建 socket
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        return false;
    }

    // 设置超时（3秒）
    DWORD timeout = 3000;
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

    // 连接仪器
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (::connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return false;
    }

    m_connected = true;
    qDebug() << "Successfully connected to instrument via TCP/IP";
    return true;
}

void TCPInstrument::disconnect() {
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        m_connected = false;
        qDebug() << "Disconnected from instrument";
    }
}

void TCPInstrument::sendCommandWrite(const std::string& command) const {
    if (!m_connected) {
        throw std::runtime_error("Not connected to instrument");
    }

    std::string scpiCommand = command + "\n";  // SCPI 命令需要换行符
    if (send(m_socket, scpiCommand.c_str(), scpiCommand.length(), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send command");
    }
}

std::string TCPInstrument::sendCommandQuery(const std::string& command) const {
    if (!m_connected) {
        throw std::runtime_error("Not connected to instrument");
    }

    sendCommandWrite(command);  // 先发送查询命令

    char buffer[1024];
    int bytesRead = recv(m_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == SOCKET_ERROR) {
        throw std::runtime_error("Failed to read response");
    }

    buffer[bytesRead] = '\0';  // 确保字符串终止
    return std::string(buffer);
}

// --- SCPI 命令封装 ---
std::string TCPInstrument::getID() {
    return sendCommandQuery("*IDN?");
}

void TCPInstrument::defineStartFreq(int freq) {
    sendCommandWrite(":SENSe:FREQuency:STARt " + std::to_string(freq));
}

void TCPInstrument::defineStopFreq(int freq) {
    sendCommandWrite(":SENSe:FREQuency:STOP " + std::to_string(freq));
}

void TCPInstrument::defineRefLevel(double RefLeveldbm) {
    sendCommandWrite(":DISPlay:WINDow:TRACe:Y:SCALe:RLEVel " + std::to_string(RefLeveldbm));
}

std::string TCPInstrument::readX() {
    return sendCommandQuery(":CALCulate:Marker1:X?");
}

void TCPInstrument::peakSearch() {
    // 示例：发送峰值搜索命令
    sendCommandWrite(":CALCulate:Marker1:maximum:PEAK");
}
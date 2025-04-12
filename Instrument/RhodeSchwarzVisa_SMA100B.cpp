#include "RhodeSchwarzVisa_SMA100B.h"
#include <QDebug>

RhodeSchwarzVisa_SMA100B::RhodeSchwarzVisa_SMA100B()
        : m_defaultRM(VI_NULL), m_session(VI_NULL) {
    ViStatus status = viOpenDefaultRM(&m_defaultRM);
    if (status < VI_SUCCESS) {
        qDebug() << "Failed to open VISA resource manager" << status;
    }
}

RhodeSchwarzVisa_SMA100B::~RhodeSchwarzVisa_SMA100B() {
    if (m_session != VI_NULL) {
        viClose(m_session);
    }
    if (m_defaultRM != VI_NULL) {
        viClose(m_defaultRM);
    }
}

void RhodeSchwarzVisa_SMA100B::connect(const std::string& resourceString) {
    if (m_session != VI_NULL) {
        viClose(m_session);
        m_session = VI_NULL;
    }

    ViStatus status = viOpen(m_defaultRM, const_cast<ViRsrc>(resourceString.c_str()),
                             VI_NULL, VI_NULL, &m_session);
    if (status < VI_SUCCESS) {
        qDebug() << "Failed to open VISA session to instrument" << status;
    }

    status = viSetAttribute(m_session, VI_ATTR_TMO_VALUE, 1000);
    if (status < VI_SUCCESS) {
        viClose(m_session);
        m_session = VI_NULL;
        qDebug() << "Failed to set VISA timeout" << status;
    }
}

void RhodeSchwarzVisa_SMA100B::disconnect() {
    if (m_session != VI_NULL) {
        ViStatus status = viClose(m_session);
        m_session = VI_NULL;  // 重置会话句柄

        if (status < VI_SUCCESS) {
            qDebug() << "Failed to properly close VISA session: " << status;
        } else {
            qDebug() << "Successfully disconnected from instrument";
        }
    } else {
        qDebug() << "No active connection to disconnect";
    }
}


std::string RhodeSchwarzVisa_SMA100B::sendCommand(const std::string& command) {
    if (m_session == VI_NULL) {
        throw std::runtime_error("Not connected to instrument");
    }

    ViUInt32 retCount;
    ViStatus status = viWrite(m_session, (ViBuf)command.c_str(),
                              (ViUInt32)command.length(), &retCount);
    if (status < VI_SUCCESS || retCount != command.length()) {
        qDebug() << "Failed to send command" << status;
    }

    char buffer[1024];
    status = viRead(m_session, (ViBuf)buffer, sizeof(buffer) - 1, &retCount);
    if (status < VI_SUCCESS) {
        if (status == VI_ERROR_TMO) {
            qDebug() << "Timeout while reading response" << status;
        }
        qDebug() << "Failed to read response" << status;
    }

    return std::string(buffer, retCount);
}

void RhodeSchwarzVisa_SMA100B::sendCommandWrite(const std::string& command) {
    if (m_session == VI_NULL) {
        throw std::runtime_error("Not connected to instrument");
    }

    ViUInt32 retCount;
    ViStatus status = viWrite(m_session, (ViBuf)command.c_str(),
                              (ViUInt32)command.length(), &retCount);
    if (status < VI_SUCCESS || retCount != command.length()) {
        qDebug() << "Failed to send command" << status;
    }
}

std::string RhodeSchwarzVisa_SMA100B::getID() {
    return sendCommand("*IDN?\n");
}

void RhodeSchwarzVisa_SMA100B::SetFrequency(int freq) {
    sendCommandWrite(":SOURce1:FREQuency:CW " + std::to_string(freq) + "\n");
}

// 0-20
void RhodeSchwarzVisa_SMA100B::setAMP(double amp_dbm) {
    sendCommandWrite(":SOURce1:POWer:LEVel:IMMediate:AMPLitude " + std::to_string(amp_dbm) + "\n");
}

void RhodeSchwarzVisa_SMA100B::SetOutput1Status(bool on) {
    sendCommandWrite(":OUTPut1:STATe " + (on ? std::to_string(1) : std::to_string(0)) + "\n");
}
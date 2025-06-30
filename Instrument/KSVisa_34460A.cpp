#include "KSVisa_34460A.h"

KSVisa_34460A::KSVisa_34460A() : m_defaultRM(VI_NULL), m_session(VI_NULL)
{
    ViStatus status = viOpenDefaultRM(&m_defaultRM);
    if (status < VI_SUCCESS) {
        qDebug() << "Failed to open VISA resource manager" << status;
    }
}

KSVisa_34460A::~KSVisa_34460A()
{
    if (m_session != VI_NULL) {
        viClose(m_session);
    }
    if (m_defaultRM != VI_NULL) {
        viClose(m_defaultRM);
    }
}


bool KSVisa_34460A::connect(const std::string& resourceString){
    if (m_session != VI_NULL) {
        viClose(m_session);
        m_session = VI_NULL;
    }

    ViStatus status = viOpen(m_defaultRM, const_cast<ViRsrc>(resourceString.c_str()),
                             VI_NULL, VI_NULL, &m_session);
    if (status < VI_SUCCESS) {
        qDebug() << "Failed to open VISA session to instrument" << status;
        return false;
    }

    status = viSetAttribute(m_session, VI_ATTR_TMO_VALUE, 3000);
    if (status < VI_SUCCESS) {
        viClose(m_session);
        m_session = VI_NULL;
        qDebug() << "Failed to set VISA timeout" << status;
        return false;
    }

    return true;
}

void KSVisa_34460A::disconnect() {
    if (m_session != VI_NULL) {
        ViStatus status = viClose(m_session);
        m_session = VI_NULL;  // Reset session handle

        if (status < VI_SUCCESS) {
            qDebug() << "Failed to properly close VISA session: " << status;
        } else {
            qDebug() << "Successfully disconnected from instrument";
        }
    } else {
        qDebug() << "No active connection to disconnect";
    }
}


void KSVisa_34460A::sendCommandWrite(const std::string& command) {
    if (m_session == VI_NULL) {
        qDebug() << "Not connected to instrument";
    }

    ViUInt32 retCount;
    ViStatus status = viWrite(m_session, (ViBuf)command.c_str(),
                              (ViUInt32)command.length(), &retCount);
    if (status < VI_SUCCESS || retCount != command.length()) {
        qDebug() << "Failed to send command" << status;
    }
}

std::string KSVisa_34460A::sendCommand(const std::string& command) {
    if (m_session == VI_NULL) {
        qDebug() << "Not connected to instrument";
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

std::string KSVisa_34460A::getID() {
    return sendCommand("*IDN?");
}

double KSVisa_34460A::readVoltage()
{
    if (m_session == VI_NULL) {
        qDebug() << "Not connected to instrument";
        return 0.0;
    }

//    std::string command = "CONF:VOLT:DC 1";
//    sendCommandWrite(command);
//    command = "SAMP:COUN 1?";
//    sendCommandWrite(command);
    std::string command = "READ?";
    std::string response = sendCommand(command);
    
    try {
        return std::stod(response);
    } catch (const std::invalid_argument& e) {
        qDebug() << "Invalid response from instrument:" << response.c_str();
        return 0.0;
    }
}


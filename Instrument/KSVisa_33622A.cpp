#include "KSVisa_33622A.h"

KeySightVisa_33622A::KeySightVisa_33622A() : m_defaultRM(VI_NULL), m_session(VI_NULL)
{
    ViStatus status = viOpenDefaultRM(&m_defaultRM);
    if (status < VI_SUCCESS) {
        qDebug() << "Failed to open VISA resource manager" << status;
    }
}

KeySightVisa_33622A::~KeySightVisa_33622A()
{
    if (m_session != VI_NULL) {
        viClose(m_session);
    }
    if (m_defaultRM != VI_NULL) {
        viClose(m_defaultRM);
    }
}

bool KeySightVisa_33622A::connect(const std::string& resourceString) {
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

    status = viSetAttribute(m_session, VI_ATTR_TMO_VALUE, 1000);
    if (status < VI_SUCCESS) {
        viClose(m_session);
        m_session = VI_NULL;
        qDebug() << "Failed to set VISA timeout" << status;
        return false;
    }

    return true;
}

void KeySightVisa_33622A::disconnect() {
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


void KeySightVisa_33622A::sendCommandWrite(const std::string& command) {
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

std::string KeySightVisa_33622A::sendCommand(const std::string& command) {
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

std::string KeySightVisa_33622A::getID() {
    return sendCommand("*IDN?");
}

void KeySightVisa_33622A::setOutputStatus(int channel, bool on){

    if (channel < 1 || channel > 2) {
        qDebug() << "Invalid channel number. Must be 1 or 2.";
        return;
    }
    std::string command = "OUTPut" + std::to_string(channel) + " " + (on ? std::to_string(1) : std::to_string(0)) + "\n";
    sendCommandWrite(command);
}

void KeySightVisa_33622A::setFrequency(int source, double freq) {
    std::string command = "SOURce" + std::to_string(source) + ":" + 
                            "FREQuency " + FreqToScientific(freq) + "\n";
    // qDebug() << command.c_str();
    sendCommandWrite(command);
}

std::string KeySightVisa_33622A::getFrequency(int source) {
    std::string command = "SOURce" + std::to_string(source) + ":" + 
                            "FREQuency?""\n";
    return sendCommand(command);
}

void KeySightVisa_33622A::setFunc(int source, std::string func) {
    std::string command = "SOURce" + std::to_string(source) + ":FUNCtion " + func + "\n";
    sendCommandWrite(command);
}

std::string KeySightVisa_33622A::getFunc(int source, std::string func) {
    std::string command = "SOURce" + std::to_string(source) + ":FUNCtion?" + func + "\n";
    return sendCommand(command);
}


// 设置输出振幅
void KeySightVisa_33622A::setVoltage(int source, double Voltage) {
    std::string command = "SOURce" + std::to_string(source) + ":VOLTage " + AmplitudeToShowpos(Voltage) + "\n";
    sendCommandWrite(command);
}

void KeySightVisa_33622A::setOffsetVoltage(int source, double Voltage) {
    std::string command = "SOURce" + std::to_string(source) + ":VOLTage:OFFSet " + AmplitudeToShowpos(Voltage) + "\n";
    sendCommandWrite(command);
}

void KeySightVisa_33622A::setHighVoltage(int source, double Voltage) {
    std::string command = "SOURce" + std::to_string(source) + ":VOLTage:HIGH " + AmplitudeToShowpos(Voltage) + "\n";
    sendCommandWrite(command);
}

void KeySightVisa_33622A::setLowVoltage(int source, double Voltage) {
    std::string command = "SOURce" + std::to_string(source) + ":VOLTage:LOW " + AmplitudeToShowpos(Voltage) + "\n";
    sendCommandWrite(command);
}
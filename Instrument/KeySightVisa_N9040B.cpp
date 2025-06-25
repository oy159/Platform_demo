#include "KeySightVisa_N9040B.h"
#include <QDebug>

KeySightVisa_N9040B::KeySightVisa_N9040B() : m_defaultRM(VI_NULL), m_session(VI_NULL) {
    ViStatus status = viOpenDefaultRM(&m_defaultRM);
    if (status < VI_SUCCESS) {
        qDebug() << "Failed to open VISA resource manager" << status;
    }
}

KeySightVisa_N9040B::~KeySightVisa_N9040B() {
    if (m_session != VI_NULL) {
        viClose(m_session);
    }
    if (m_defaultRM != VI_NULL) {
        viClose(m_defaultRM);
    }
}

bool KeySightVisa_N9040B::connect(const std::string& resourceString) {
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

void KeySightVisa_N9040B::disconnect() {
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

void KeySightVisa_N9040B::sendCommandWrite(const std::string& command) const {
    if (m_session == VI_NULL) {
        throw std::runtime_error("Not connected to instrument");
    }

    ViUInt32 retCount;
    ViStatus status = viWrite(m_session, (ViBuf)command.c_str(),
                              (ViUInt32)command.length(), &retCount);
    if (status < VI_SUCCESS || retCount != command.length()) {
        throw std::runtime_error("Failed to send command");
    }
}

std::string KeySightVisa_N9040B::sendCommandQuery(const std::string& command) const {
    if (m_session == VI_NULL) {
        throw std::runtime_error("Not connected to instrument");
    }

    ViUInt32 retCount;
    ViStatus status = viWrite(m_session, (ViBuf)command.c_str(),
                              (ViUInt32)command.length(), &retCount);
    if (status < VI_SUCCESS || retCount != command.length()) {
        throw std::runtime_error("Failed to send command");
    }

    char buffer[1024];
    status = viRead(m_session, (ViBuf)buffer, sizeof(buffer) - 1, &retCount);
    if (status < VI_SUCCESS) {
        if (status == VI_ERROR_TMO) {
            throw std::runtime_error("Timeout while reading response");
        }
        throw std::runtime_error("Failed to read response");
    }

    return std::string(buffer, retCount);
}

std::string KeySightVisa_N9040B::getID() {
    return sendCommandQuery("*IDN?\n");
}

void KeySightVisa_N9040B::defineStartFreq(int freq) {
    sendCommandWrite(":SENSe:FREQuency:STARt " + std::to_string(freq) + "\n");
}

void KeySightVisa_N9040B::defineStopFreq(int freq) {
    sendCommandWrite(":SENSe:FREQuency:STOP " + std::to_string(freq) + "\n");
}

void KeySightVisa_N9040B::defineRefLevel(double RefLeveldbm) {
    sendCommandWrite(":DISPlay:WINDow:TRACe:Y:SCALe:RLEVel " + std::to_string(RefLeveldbm) + "\n");
}

std::string KeySightVisa_N9040B::readX() {
    return sendCommandQuery(":CALCulate:Marker1:X?\n");
}

void KeySightVisa_N9040B::peakSearch() {
    // 实现峰值搜索功能
    // sendCommandWrite(":CALCulate:Marker1:maximum:PEAK\n");
    // 其他相关命令...

    //        :CALCulate:Marker1:maximum:PEAK\n
    //        :CALCulate:Marker1:maximum:NEXT\n
    //        :CALCulate:Marker1:maximum:RIGHt\n
    //        :CALCulate:Marker1:maximum:LEFT\n


    //        :SENSe:BANDwidth:RESoulution:AUTO 0/1
    //        :SENSe:BANDwidth:RESoulution 30000       // RBW

    //        :SENSe:BANDwidth:VIDeo:AUTO 0/1
    //         :SENSe:BANDwidth:VIDeo 30000            //VBW

    //          :SENSe:BANDwidth:VIDeo:RATio:AUTO 0/1
    //          :SENSe:BANDwidth:VIDeo:RATio 1

    //          :SENSe:FREQuency:SPAN:BANDwidth:RESolution:RATio:AUTO 0/1
    //          :SENSe:FREQuency:SPAN:BANDwidth:RESolution:RATio 106

    //          :SENSE:SWEep:POINts 2048        // 点数


}


QVector<QPointF> KeySightVisa_N9040B::convertSpectrumDataToQPoints(const char* buffer, int bufferSize) {
    QVector<QPointF> points;
    
    // 将字符缓冲区转换为QString
    QString dataString = QString::fromLocal8Bit(buffer, bufferSize);
    
    // 移除可能的空格和换行符
    dataString = dataString.trimmed();
    
    // 分割字符串为单独的数据项
    QStringList dataList = dataString.split(",", Qt::SkipEmptyParts);
    
    // 检查数据数量是否有效
    if (dataList.size() != 2048) {
        qWarning() << "数据数量不匹配，期望2048个，实际得到" << dataList.size();
        return points;
    }
    
    // 转换为QPointF数组
    bool ok;
    for (int i = 0; i < dataList.size(); i += 2) {
        double frequency = dataList[i].toDouble(&ok);
        if (!ok) {
            qWarning() << "频率转换失败:" << dataList[i];
            continue;
        }
        
        double amplitude = dataList[i+1].toDouble(&ok);
        if (!ok) {
            qWarning() << "幅度转换失败:" << dataList[i+1];
            continue;
        }
        
        points.append(QPointF(frequency, amplitude));
    }
    
    return points;
}
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
    sendCommandWrite(":SENSe:FREQuency:STARt " + std::to_string(freq) + "\n"); // general
}

void KeySightVisa_N9040B::defineStopFreq(int freq) {
    sendCommandWrite(":SENSe:FREQuency:STOP " + std::to_string(freq) + "\n"); // general
}

void KeySightVisa_N9040B::defineRefLevel(double RefLeveldbm) {
#ifdef USE_RSA3030N
    sendCommandWrite(":DISPlay:WINDow:TRACe:Y:SCALe:RLEVel:OFFSet " + std::to_string(RefLeveldbm) + "\n"); // RSA3030N
#else
    sendCommandWrite(":DISPlay:WINDow:TRACe:Y:SCALe:RLEVel " + std::to_string(RefLeveldbm) + "\n"); // N9040B
#endif
}

std::string KeySightVisa_N9040B::readX() {
    return sendCommandQuery(":CALCulate:Marker1:X?\n");     // general
}

void KeySightVisa_N9040B::peakSearch(PeakSearchMode mode) {
    // 实现峰值搜索功能
    // sendCommandWrite(":CALCulate:Marker1:maximum:PEAK\n");
    // 其他相关命令...

    switch (mode)
    {
    case  PeakSearch:
        /* code */
        sendCommandWrite(":CALCulate:Marker1:MAXimum:PEAK\n");
        break;

    case NextSearch:
        sendCommandWrite(":CALCulate:Marker1:MAXimum:NEXT\n");
        break;

    case RightSearch:
        sendCommandWrite(":CALCulate:Marker1:MAXimum:RIGHt\n");
        break;

    case LeftSearch:
        sendCommandWrite(":CALCulate:Marker1:MAXimum:LEFT\n");
        break;
    
    default:
        qDebug() << "Unknown PeakSearchMode:" << mode;
        break;
    }

    std::string dataString = readX();
    qDebug() << "Peak Search Result:" << QString::fromStdString(dataString);

    // dataSting 格式


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


QVector<QPointF> KeySightVisa_N9040B::readSA(){
    std::string command = ":READ:SANalyzer1?\n";
    if (m_session == VI_NULL) {
        throw std::runtime_error("Not connected to instrument");
    }


    auto status = viSetAttribute(m_session, VI_ATTR_TMO_VALUE, 8000); 
    if (status < VI_SUCCESS) {
        viClose(m_session);
        m_session = VI_NULL;
        qDebug() << "Failed to set VISA timeout" << status;
    }

    ViUInt32 retCount;
    status = viWrite(m_session, (ViBuf)command.c_str(),
                              (ViUInt32)command.length(), &retCount);
    if (status < VI_SUCCESS || retCount != command.length()) {
        throw std::runtime_error("Failed to send command");
    }

    char buffer[81920]; // 增大缓冲区以适应更大的数据量
    status = viRead(m_session, (ViBuf)buffer, sizeof(buffer) - 1, &retCount);
    if (status < VI_SUCCESS) {
        if (status == VI_ERROR_TMO) {
            throw std::runtime_error("Timeout while reading response");
        }
        throw std::runtime_error("Failed to read response");
    }
    qDebug() << buffer;
    return convertSpectrumDataToQPoints(buffer, retCount);
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
//    if (dataList.size() != 2048) {
//        qWarning() << "数据数量不匹配，期望2048个，实际得到" << dataList.size();
//        return points;
//    }
    
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


// // 生成模拟频谱数据 (1024个点，2048个值)
// std::string generateSpectrumData() {
//     std::string data;
//     char buffer[50];
    
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_real_distribution<double> amp_dist(1e-5, 1e-3);
    
//     double freq = 1.0e7;  // 起始频率 10 MHz
    
//     for (int i = 0; i < 1024; i++) {
//         // 生成频率值 (科学计数法)
//         snprintf(buffer, sizeof(buffer), "%.7E", freq);
//         data += buffer;
//         data += ",";
        
//         // 生成幅度值 (科学计数法)
//         double amplitude = amp_dist(gen);
//         snprintf(buffer, sizeof(buffer), "%.7E", amplitude);
//         data += buffer;
        
//         // 添加分隔符 (最后一个数据后不加逗号)
//         if (i < 1023) data += ",";
        
//         // 增加频率
//         freq += 1e5;  // 每次增加 100 kHz
//     }
    
//     data += "\n";  // 以换行符结尾
//     return data;
// }

// QVector<QPointF> convertSpectrumDataToQPoints(const char* buffer, int bufferSize) {
//     QVector<QPointF> points;
    
//     // 将字符缓冲区转换为QString
//     QString dataString = QString::fromLocal8Bit(buffer, bufferSize);
    
//     // 移除可能的空格和换行符
//     dataString = dataString.trimmed();
    
//     // 分割字符串为单独的数据项
//     QStringList dataList = dataString.split(",", Qt::SkipEmptyParts);
    
//     // 检查数据数量是否有效
//     if (dataList.size() != 2048) {
//         qWarning() << "数据数量不匹配，期望2048个，实际得到" << dataList.size();
//         return points;
//     }
    
//     // 转换为QPointF数组
//     bool ok;
//     for (int i = 0; i < dataList.size(); i += 2) {
//         double frequency = dataList[i].toDouble(&ok);
//         if (!ok) {
//             qWarning() << "频率转换失败:" << dataList[i];
//             continue;
//         }
        
//         double amplitude = dataList[i+1].toDouble(&ok);
//         if (!ok) {
//             qWarning() << "幅度转换失败:" << dataList[i+1];
//             continue;
//         }
        
//         points.append(QPointF(frequency, amplitude));
//     }
    
//     return points;
// }

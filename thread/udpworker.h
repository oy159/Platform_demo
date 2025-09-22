#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QThreadPool>
#include <QRunnable>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>


// command list
typedef enum{
    CMD_GET_FIRMWARE_VERSION = 0xff7fff7f,
    CMD_SET_AD9268_CHANNEL1 = 0xff7fff81,
    CMD_SET_AD9268_CHANNEL2 = 0xff7fff82,
    CMD_ENABLE_AD9915_EXTERNAL_CLOCK = 0xff7fff83,
    CMD_ENABLE_AD9915_INTERNAL_CLOCK = 0xff7fff84,
    CMD_SET_DDS_FREQ = 0xff7fff85,
    CMD_SET_DAC_VALUE = 0xff7fff86,
    CMD_SET_DDS_TWO_TONE = 0xff7fff87,
    CMD_GET_ZYNQ_TEMP = 0xff7fff88,
    CMD_SET_ZYNQ_AUTODETECT = 0xff7fff89,
}DeviceCommand;

typedef enum{
    UnknownDevice = 0,
    AD9434 = 1,
    AD9268 = 2,
    AD9142 = 3,
    AD9747 = 4,
    AutoDetect = 5
} DeviceType;

class UdpWorker : public QObject{
Q_OBJECT

public:
    explicit UdpWorker(QObject *parent = nullptr);
    ~UdpWorker();

public slots:
    void connectToHost(const QString &ip, int remote_port, int local_port);
    void disconnectFromHost();
    void sendMessage(const QString &message);
    void handleClearADCData();
    void handleSetAD9268Channel(int channel);
    void handleSetAD9518ExternalClock(int freq_Hz);
    void handleSetAD9518InternalClock();
    void handleSetDDSFreq(int freq_Hz, int Mode = 1);
    void handleSetDACValue(int value);
    void handleGetTemp();
    void handleGetAutoDetect();


signals:
    void errorOccurred(const QString &error);
    void dataReceived(const QByteArray &data);
    void ADCDataReady(const std::vector<uint16_t> &data);
    void initializePlatform(const QString &DeviceName);
    void DACValueSetSuccess();
    void sendTemp(const QString &Temp);
    void autoDetectResult(bool result);

private slots:
    void handleReadyRead();
    void handleDataReceived(const QByteArray &data);
    

private:
    void convertBufferToU16Array(const QByteArray &buffer, std::vector<uint16_t> &u16Array);
    QUdpSocket *udpSocket;
    QUdpSocket *udpSocket2;
    QByteArray buffer;
    QTimer *tempTimer;
//    QThreadPool *threadPool;
    QEventLoop eventLoop;
    QString target_ip;
    uint16_t remote_port;
    uint16_t  local_port;

    std::vector<uint16_t> AdcDataArray;
    bool transFinished = false;

    bool is_framehead = false;
    DeviceCommand current_command = CMD_GET_FIRMWARE_VERSION;
    DeviceType mdeviceType = UnknownDevice;

};

#define BIG_LITTLE_SWAP16(x)        ( (((*(short int *)&x) & 0xff00) >> 8) | \
                                      (((*(short int *)&x) & 0x00ff) << 8) )

#define XAdcPs_RawToTemperature(AdcData)				\
	((((float)(AdcData)/65536.0f)/0.00198421639f ) - 273.15f)

#define XAdcPs_RawToVoltage(AdcData) 					\
	((((float)(AdcData))* (3.0f))/65536.0f)

// typedef enum{
//     ADC_DYNAMIC_MODE = 0,
//     ADC_STATIC_MODE = 1,
//     DAC_DYNAMIC_MODE = 2,
//     DAC_STATIC_MODE = 3
// }CaculateMode;


#endif // UDPWORKER_H
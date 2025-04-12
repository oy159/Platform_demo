#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QThreadPool>
#include <QRunnable>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

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


signals:
    void errorOccurred(const QString &error);
    void dataReceived(const QByteArray &data);
    void ADCDataReady(const std::vector<uint16_t> &data);

private slots:
    void handleReadyRead();
    void handleDataReceived(const QByteArray &data);
    

private:
    void convertBufferToU16Array(const QByteArray &buffer, std::vector<uint16_t> &u16Array);
    QUdpSocket *udpSocket;
    QByteArray buffer;
    QTimer *timer;
    QThreadPool *threadPool;
    QEventLoop eventLoop;
    QString target_ip;
    uint16_t remote_port;
    uint16_t  local_port;

    std::vector<uint16_t> AdcDataArray;
    bool transFinished = false;

    bool is_framehead = false;

};

#define BIG_LITTLE_SWAP16(x)        ( (((*(short int *)&x) & 0xff00) >> 8) | \
                                      (((*(short int *)&x) & 0x00ff) << 8) )


// typedef enum{
//     ADC_DYNAMIC_MODE = 0,
//     ADC_STATIC_MODE = 1,
//     DAC_DYNAMIC_MODE = 2,
//     DAC_STATIC_MODE = 3
// }CaculateMode;


#endif // UDPWORKER_H
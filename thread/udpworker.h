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


signals:
    void messageReceived(const QByteArray &message);
    void errorOccurred(const QString &error);

private slots:
    void handleReadyRead();

private:
    QUdpSocket *udpSocket;
    QByteArray buffer;
    QTimer *timer;
    QThreadPool *threadPool;
    QEventLoop eventLoop;
    QString target_ip;
    uint16_t remote_port;
    uint16_t  local_port;
};

#endif // UDPWORKER_H
#include <QApplication>
#include "platform_demo_test.h"
#include "fftw3.h"
#include "NewChart.h"
#include "testdata.h"
#include "BaseChartWidget.h"
#include "SpectrumChartTryWidget.h"
#include "conf.h"

QFile logFile;
QMutex logMutex;

void cleanOldLogs(const QString& logDirPath, int maxFiles) {
    QDir logDir(logDirPath);
    QStringList logFiles = logDir.entryList(QStringList() << "*.log", QDir::Files, QDir::Time);
    if (logFiles.size() > maxFiles) {
        for (int i = maxFiles; i < logFiles.size(); ++i) {
            logDir.remove(logFiles[i]);
        }
    }
}

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&logMutex);
    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString typeStr;
    switch (type) {
        case QtDebugMsg:    typeStr = "[DEBUG]"; break;
        case QtWarningMsg:  typeStr = "[WARNING]"; break;
        case QtCriticalMsg: typeStr = "[ERROR]"; break;
        case QtFatalMsg:    typeStr = "[FATAL]"; break;
        default:            typeStr = "[INFO]"; break;
    }
    QString logLine = timeStr + " " + typeStr + " " + msg + "\n";

#if PLATFORM_DEMO_LOG_MODE == 0
    fprintf(stdout, "%s", logLine.toLocal8Bit().constData());
#elif PLATFORM_DEMO_LOG_MODE == 1
    if (logFile.isOpen()) {
        QTextStream out(&logFile);
        out << logLine;
        out.flush();
    }
#elif PLATFORM_DEMO_LOG_MODE == 2
    fprintf(stdout, "%s", logLine.toLocal8Bit().constData());
    if (logFile.isOpen()) {
        QTextStream out(&logFile);
        out << logLine;
        out.flush();
    }
#endif
}


int main(int argc, char *argv[]) {

    // 日志目录和文件名
    QString logDirPath = "./log";
    QDir().mkpath(logDirPath);
    QString timeStr = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
    QString logFileName = QString("%1/platform_demo-%2.log").arg(logDirPath).arg(timeStr);

    // 清理旧日志
    cleanOldLogs(logDirPath, MAX_LOG_FILES);

    // 打开日志文件
    logFile.setFileName(logFileName);
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);

    qInstallMessageHandler(myMessageHandler);


    QApplication a(argc, argv);

    QFile styleFile(":/qss/themes/MacOS.qss"); // 使用资源文件路径，或者绝对路径、相对路径
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        a.setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qWarning() << "Failed to open style sheet file.";
    }


    platform_demo_test w;
    w.show();

    return a.exec();

}

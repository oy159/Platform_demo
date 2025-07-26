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
    platform_demo_test w;
    w.show();

    SpectrumChartTryWidget w2;
    w2.setSampleRate(100e6); // 100 MHz采样率
    w2.handleRefreshSpectrum(adc16Data);
    w2.show();
    

    

    // SpectrumChartView viewer;
    // viewer.resize(800, 600);
    // viewer.show();
    
    return a.exec();

}

// todo: 添加识别当前设备功能
// todo: 编写adc的评估代码
// todo：优化界面

//#include <iostream>
//#include <cmath>
//#include <vector>

//int main() {
//    // 参数设置
//    const double sample_rate = 44100.0;  // 采样率 (Hz)
//    const double duration = 0.1;         // 信号时长 (秒)
//    const double freq = 1000.0;          // 正弦波频率 (Hz)
//    const size_t N = static_cast<size_t>(sample_rate * duration);  // 采样点数
//
//    // 1. 生成正弦波信号
//    std::vector<double> signal(N);
//    for (size_t i = 0; i < N; ++i) {
//        double t = i / sample_rate;
//        signal[i] = sin(2.0 * M_PI * freq * t);  // 生成正弦波
//    }
//
//    // 2. 准备 FFT 输入/输出
//    fftw_complex* fft_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
//    fftw_complex* fft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
//
//    // 将信号填充到 FFT 输入（复数格式，虚部为 0）
//    for (size_t i = 0; i < N; ++i) {
//        fft_in[i][0] = signal[i];  // 实部
//        fft_in[i][1] = 0.0;        // 虚部
//    }
//
//    // 3. 创建 FFT 计划并执行
//    fftw_plan plan = fftw_plan_dft_1d(N, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE);
//    fftw_execute(plan);
//
//    // 4. 计算幅度谱并找到峰值频率
//    double max_magnitude = 0.0;
//    size_t fund_peakIndex = 0;
//
//    for (size_t i = 0; i < N / 2; ++i) {  // 只需检查前一半（对称性）
//        double real = fft_out[i][0];
//        double imag = fft_out[i][1];
//        double magnitude = sqrt(real * real + imag * imag);  // 计算幅度
//
//        if (magnitude > max_magnitude) {
//            max_magnitude = magnitude;
//            fund_peakIndex = i;
//        }
//    }
//
//    // 5. 计算峰值对应的频率
//    double peak_freq = fund_peakIndex * sample_rate / N;
//
//    // 输出结果
//    std::cout << "Generated signal frequency: " << freq << " Hz\n";
//    std::cout << "Detected peak frequency:    " << peak_freq << " Hz\n";
//
//    // 6. 清理资源
//    fftw_destroy_plan(plan);
//    fftw_free(fft_in);
//    fftw_free(fft_out);
//
//    return 0;
//}
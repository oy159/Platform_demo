#ifndef SPECTRUMCHARTWIDGET_H
#define SPECTRUMCHARTWIDGET_H

#include "BaseChartWidget.h"
#include "thread/CaculateParams.h"
#include <QScatterSeries>
#include <QLineSeries>
#include "callout.h"

class SpectrumChartTryWidget : public BaseChartWidget {
    Q_OBJECT
public:
    explicit SpectrumChartTryWidget(QWidget *parent = nullptr);
    
    // 设置采样率和频率模式
    void setSampleRate(double rate);
    void setFrequencyMode(bool enable);

    virtual void addCustomContextMenuActions(QMenu *menu);
    
    // 频谱处理槽函数
public slots:
    void handleRefreshPeakData(const std::vector<Peak>& peaks);
    void handleRefreshSpectrum(std::vector<double> fft_data);
    void handleFindPeak();
    void handleFindNextPeak();

protected:
    // 重写基类方法
    // void optimizeAxisRanges(const QVector<QPointF> &data) override;
    void keyPressEvent(QKeyEvent *event) override;

    // void contextMenuEvent(QContextMenuEvent *event) override;

private:
    // 标记特定峰值点
    void markPeak(int index);
    
    // 频谱特有属性
    double m_sampleRate = 1e8;          // 采样率
    bool m_frequencyMode = false;       // 是否使用频率模式
    int m_sampleNum = 0;                // 采样点数
    int m_peakCnt = 0;                  // 当前显示的峰值索引
    
    std::vector<Peak> m_peaks;
    QVector<QPointF> chart_data;
    

    QScatterSeries *m_peakSeries = nullptr;

    // 峰值标注
    Callout *m_peakCallout = nullptr;   // 峰值标注


};

#endif // SPECTRUMCHARTWIDGET_H
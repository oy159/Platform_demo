//
// Created by oycr on 2025/4/4.
//

#ifndef PLATFORM_DEMO_SPECTRUMCHARTWIDGET_H
#define PLATFORM_DEMO_SPECTRUMCHARTWIDGET_H

#include <QWidget>
#include <QLineSeries>
#include <QValueAxis>
#include <QSplineSeries>
#include <QLabel>
#include "QChartView"
#include "QSlider"
#include <QScatterSeries>
#include <QInputDialog>
#include <QApplication>
#include "thread/CaculateParams.h"
#include <QRubberBand>
#include <QMessageBox>
#include "callout.h"
#include "NavigationChartView.h"


class SpectrumChartWidget: public QWidget{
    Q_OBJECT
public:
    explicit SpectrumChartWidget(QWidget *parent = nullptr);
    ~SpectrumChartWidget();

    void updateCoordText(const QPointF &chartPos);
    void setSampleRate(double rate) { 
        sampleRate = rate; set_freq = true; 
        axisX->setTitleText("MHz");
        axisX->setLabelFormat("%.3f");
        firstDataReceived = true;
    }

public slots:
    // 接收新数据并更新图表
    void updateWaveformData(const QVector<QPointF> &newData);
    void handleRefreshPeakData(const std::vector<Peak>& peaks);
    void onSliderValueChanged(int value);
    void handleRefreshSpectrum(std::vector<double> fft_data);

    // 清空图表数据
//    void clearWaveformData();

signals:
    // 数据请求信号
    void requestData();

private slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void initChart();
    void optimizeAxisRanges(const QVector<QPointF> &data);
    QImage circle(qreal imageSize, const QColor &color);
    

    double recoverX = 0; // 恢复的X轴范围
    double recoverY = 0; // 恢复的Y轴范围

    double sampleRate = 1e8; // 默认采样率
    bool set_freq = false; // 是否设置了频率
    int sampleNum = 0; // 采样点数


    NavigationChartView *chartView;
    QChart *chart;
    QLineSeries *series;
    QScatterSeries *peakSeries;
    QValueAxis *axisX;
    QValueAxis *axisY;

    std::vector<Peak> mpeaks;

    // QLabel *counterLabel;  // 新增：计数标签
    int updateCount = 0;   // 新增：更新计数器
    QLabel *coordinateLabel;

    bool firstDataReceived = true;

    QList<Callout *> m_callouts;
    Callout *m_tooltip = nullptr;
    QGraphicsSimpleTextItem *m_coordX = nullptr;
    QGraphicsSimpleTextItem *m_coordY = nullptr;

};





#endif //PLATFORM_DEMO_CHARTWIDGET_H

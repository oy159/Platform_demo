//
// Created by oycr on 2025/4/4.
//

#ifndef PLATFORM_DEMO_CHARTWIDGET_H
#define PLATFORM_DEMO_CHARTWIDGET_H

#include <QWidget>
#include <QLineSeries>
#include <QValueAxis>
#include <QLabel>
#include "QChartView"
#include "QSlider"
#include <QScatterSeries>
#include <QInputDialog>
#include <QMessageBox>
#include "thread/CaculateParams.h"



class mSlider: public QSlider {
    Q_OBJECT
public:
    explicit mSlider(QWidget *parent = nullptr);
    ~mSlider() override;

private:
    void mousePressEvent(QMouseEvent *ev) override;
};


class ChartWidget: public QWidget{
    Q_OBJECT
public:
public:
    explicit ChartWidget(QWidget *parent = nullptr);
    ~ChartWidget();

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

private:
    void initChart();
    void optimizeAxisRanges(const QVector<QPointF> &data);
    void AdjustAxisXRange(double min, double max);
    void showPointCoordinates(const QPointF &point);

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;   
    void keyPressEvent(QKeyEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;
    void updateCounterLabel();

    bool isDragging = false;
    QPointF dragStartPos;


    QChartView *chartView;
    QChart *chart;
    QLineSeries *series;
    QScatterSeries *peakSeries;
    QValueAxis *axisX;
    QValueAxis *axisY;

    std::vector<Peak> mpeaks;

    QLabel *counterLabel;  // 新增：计数标签
    int updateCount = 0;   // 新增：更新计数器
    QLabel *coordinateLabel;
    mSlider* slider;

    bool firstDataReceived = true;


};





#endif //PLATFORM_DEMO_CHARTWIDGET_H

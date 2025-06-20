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


class NavigationChartView : public QChartView {
public:
    using QChartView::QChartView;
    
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::MiddleButton) {
            m_lastPos = event->pos();
            setCursor(Qt::ClosedHandCursor);
            return;
        }
        QChartView::mousePressEvent(event);
    }
    
    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::MiddleButton) {
            auto dPos = event->pos() - m_lastPos;
            chart()->scroll(-dPos.x(), dPos.y());
            m_lastPos = event->pos();
            return;
        }
        QChartView::mouseMoveEvent(event);
    }
    
    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::MiddleButton) {
            setCursor(Qt::ArrowCursor);
            return;
        }
        
        // 自定义右键行为
        if (event->button() == Qt::RightButton) {
            if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
                chart()->zoomReset(); // Shift+右键 复位视图
            } else {
                return;
                // chart()->zoom(0.7); // 普通右键缩小更多
            }
            return;
        }
        
        QChartView::mouseReleaseEvent(event);
    }
    
    void wheelEvent(QWheelEvent *event) override {
        double factor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
        chart()->zoom(factor);
    }
    
private:
    QPoint m_lastPos;
};


class mSlider: public QSlider {
    Q_OBJECT
public:
    explicit mSlider(QWidget *parent = nullptr);
    ~mSlider() override;

private:
    void mousePressEvent(QMouseEvent *ev) override;
};


class SpectrumChartWidget: public QWidget{
    Q_OBJECT
public:
public:
    explicit SpectrumChartWidget(QWidget *parent = nullptr);
    ~SpectrumChartWidget();

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
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void initChart();
    void optimizeAxisRanges(const QVector<QPointF> &data);
    void AdjustAxisXRange(double min, double max);
    void showPointCoordinates(const QPointF &point);

    // void wheelEvent(QWheelEvent *event) override;
    

    // void updateMarkerInfo();
    // void resizeEvent(QResizeEvent *event) override;
    // void mousePressEvent(QMouseEvent *event) override;
    // void mouseMoveEvent(QMouseEvent *event) override;
    // void mouseReleaseEvent(QMouseEvent *event) override;
    
    void keyPressEvent(QKeyEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;
    void updateCounterLabel();

    QImage circle(qreal imageSize, const QColor &color);

    bool isDragging = false;
    QPointF dragStartPos;


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
    // mSlider* slider;

    bool firstDataReceived = true;

    QList<Callout *> m_callouts;
    Callout *m_tooltip = nullptr;
    QGraphicsSimpleTextItem *m_coordX = nullptr;
    QGraphicsSimpleTextItem *m_coordY = nullptr;

};





#endif //PLATFORM_DEMO_CHARTWIDGET_H

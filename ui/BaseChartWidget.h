#ifndef BASECHARTWIDGET_H
#define BASECHARTWIDGET_H

#include <QWidget>
#include <QtCharts>
#include "callout.h"
#include "NavigationChartView.h"


class BaseChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit BaseChartWidget(QWidget *parent = nullptr);
    virtual ~BaseChartWidget();
    
    // 通用接口
    virtual void updateChartData(const QVector<QPointF> &data);
    virtual void clearChartData();
    
    void setAxisLabels(const QString &xLabel, const QString &yLabel);
    void setAxisPrecision(int xPrecision, int yPrecision);
    void setXAxisRange(double min, double max);
    void setYAxisRange(double min, double max);
    void setChartTitle(const QString &title);
    void setLineColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setGridVisible(bool visible);
    void resetView();
    void updateCoordText(const QPointF &chartPos);
    void setRecoverRange(double xMin, double xMax, double yMin, double yMax);
    
    QChartView* chartView() const { return m_chartView; }

public slots:
    void updateChartDataDirect(std::vector<double> data);

protected:
    virtual void initChart();
    virtual void optimizeAxisRanges(const QVector<QPointF> &data);
    
    QChart *m_chart = nullptr;
    NavigationChartView *m_chartView = nullptr;
    QLineSeries *m_series = nullptr;
    QScatterSeries *m_markerSeries = nullptr;
    QValueAxis *m_axisX = nullptr;
    QValueAxis *m_axisY = nullptr;
    
    double m_recoverXMin = 0;
    double m_recoverXMax = 0;
    double m_recoverYMin = 0;
    double m_recoverYMax = 0;
    
    int m_xPrecision = 2;
    int m_yPrecision = 2;
    
    // 坐标显示
    QGraphicsSimpleTextItem *m_coordX = nullptr;
    QGraphicsSimpleTextItem *m_coordY = nullptr;
    Callout *m_tooltip = nullptr;
    QList<Callout *> m_callouts;

private slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);
    

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // BASECHARTWIDGET_H
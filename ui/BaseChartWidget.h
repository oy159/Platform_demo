#ifndef BASECHARTWIDGET_H
#define BASECHARTWIDGET_H

#include <QWidget>
#include <QtCharts>
#include "callout.h"
#include "NavigationChartView.h"
#include "Cursor.h"
#include "MarkerColorAction.h"

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
    void setPeakData(QVector<QPointF> Peaks);
    void setPointMarker(QPointF point);
    
    QChartView* chartView() const { return m_chartView; }

public slots:
    void updateChartDataDirect(std::vector<double> data);
    void findMaxPoint();
    void findNextPoint();

protected:
    void initChart();
    void initMenu();

    virtual void optimizeAxisRanges(const QVector<QPointF> &data);
    virtual void markPeak(int index);
    static void optimizeAxisRange(double &min, double &max, double marginRatio = 0.1);
    static double calculateNiceTickInterval(double range);
    void contextMenuEvent(QContextMenuEvent *event) override;
    void loadStyleSheet();
    void optimizeCursor() const;
    QVector<QPointF> findPeaks(const std::vector<double>& data,
                                double minHeight = -INFINITY,
                                size_t minDistance = 1);
    
    QChart *m_chart = nullptr;
    NavigationChartView *m_chartView = nullptr;
    QLineSeries *m_series = nullptr;
    QScatterSeries *m_scatterSeries = nullptr;
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

    Cursor *m_cursor = nullptr;
    QVector<QPointF> mPeaks;
    int mCurrentPeakIndex = -1;
    QVector<bool> mPeaksVisible;
    QMenu *menu = nullptr;
    QAction *findMaxAction;
    QAction *findNextAction;

protected slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);

    void captrueScreen();
    void exportData();
    void autoAdjust();
    void setCursorVisible(bool visible);
    void addMarker();

    void findPoint(int index);
    void findPoint(QPointF point);
    void setMarkerToggled(bool checked);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    bool first = true;

    QAction *snipAction;
    QAction *exportDataAction;

    QMenu *settingSubMenu;
    QAction *ControlCursorAction;
    QAction *ZoomOutAction;

    bool _ifShowCursor = false;
    QMenu *markerSubMenu;
    QVector<MarkerColorAction*> markerActions;
    MarkerColorAction *marker1Action;
    QAction *addMarkerAction;
    QAction *ClearMarkerAction;


    QMenu *axisYSubMenu;
    QWidgetAction *axisYPrecision;
    QWidget *axisYPrecisionWidget;
    QHBoxLayout *axisYPrecisionLayout;
    QLabel *axisYPrecisionLabel;
    QSpinBox *axisYPrecisionSB;

    QMenu *axisXSubMenu;
    QWidgetAction *axisXPrecision;
    QWidget *axisXPrecisionWidget;
    QHBoxLayout *axisXPrecisionLayout;
    QLabel *axisXPrecisionLabel;
    QSpinBox *axisXPrecisionSB;

    QAction *autoAdjustAction;

    // QVector<QPair<bool, int>> MarkerCalloutFlag;
    QList<Callout*> m_calloutPool;
};

#endif // BASECHARTWIDGET_H

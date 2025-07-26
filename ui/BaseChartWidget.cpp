#include "BaseChartWidget.h"
#include "callout.h"
#include <QVBoxLayout>
#include <QDebug>

BaseChartWidget::BaseChartWidget(QWidget *parent) 
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    initChart();
    mainLayout->addWidget(m_chartView);
}

BaseChartWidget::~BaseChartWidget() {
    delete m_chart;
}

void BaseChartWidget::initChart() {
    m_chart = new QChart();
    m_chart->setAnimationOptions(QChart::NoAnimation);
    m_chart->setMargins(QMargins(5, 5, 5, 5));
    
    m_series = new QLineSeries();
    m_series->setUseOpenGL(true);
    
    QPen pen(Qt::blue);
    pen.setWidthF(1.5);
    m_series->setPen(pen);
    
    m_chart->addSeries(m_series);
    
    // 初始化坐标轴
    m_axisX = new QValueAxis;
    m_axisY = new QValueAxis;
    
    m_axisX->setTitleText("X");
    m_axisX->setLabelFormat(QString("%.%1f").arg(m_xPrecision));
    m_axisX->setTickCount(10);
    
    m_axisY->setTitleText("Y");
    m_axisY->setLabelFormat(QString("%.%1f").arg(m_yPrecision));
    m_axisY->setTickCount(10);
    
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
    
    // 标记点序列
    m_markerSeries = new QScatterSeries();
    m_markerSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_markerSeries->setMarkerSize(10.0);
    m_markerSeries->setColor(Qt::red);
    m_chart->addSeries(m_markerSeries);
    m_markerSeries->attachAxis(m_axisX);
    m_markerSeries->attachAxis(m_axisY);
    
    m_chartView = new NavigationChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing, true);
    m_chartView->setRenderHint(QPainter::SmoothPixmapTransform, true);
    m_chartView->setDragMode(QGraphicsView::RubberBandDrag);
    m_chartView->setMouseTracking(true);
    m_chartView->setRubberBand(QChartView::RectangleRubberBand);
    m_chart->legend()->hide();
    
    connect(m_series, &QLineSeries::clicked, this, &BaseChartWidget::keepCallout);
    connect(m_series, &QLineSeries::hovered, this, &BaseChartWidget::tooltip);
    
    // 坐标显示
    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    // m_coordX->setText("X: ");
    m_coordY = new QGraphicsSimpleTextItem(m_chart);
    // m_coordY->setText("Y: ");
    m_coordX->setBrush(Qt::black);
    m_coordY->setBrush(Qt::black);
}

void BaseChartWidget::updateChartDataDirect(std::vector<double> data) {
    QVector<QPointF> chartData;

    int DataNum = data.size();
    
    for (size_t i = 0; i < data.size(); ++i) {
        chartData.append(QPointF(i, data[i]));
    }

    updateChartData(chartData);
}


void BaseChartWidget::updateChartData(const QVector<QPointF> &data) {
    if (data.isEmpty()) return;
    
    m_series->replace(data);
    optimizeAxisRanges(data);
}

void BaseChartWidget::clearChartData() {
    m_series->clear();
    m_markerSeries->clear();
}

void BaseChartWidget::setAxisLabels(const QString &xLabel, const QString &yLabel) {
    m_axisX->setTitleText(xLabel);
    m_axisY->setTitleText(yLabel);
}

void BaseChartWidget::setAxisPrecision(int xPrecision, int yPrecision) {
    m_xPrecision = xPrecision;
    m_yPrecision = yPrecision;
    m_axisX->setLabelFormat(QString("%.%1f").arg(m_xPrecision));
    m_axisY->setLabelFormat(QString("%.%1f").arg(m_yPrecision));
}

void BaseChartWidget::setXAxisRange(double min, double max) {
    m_axisX->setRange(min, max);
}

void BaseChartWidget::setYAxisRange(double min, double max) {
    m_axisY->setRange(min, max);
}

void BaseChartWidget::setChartTitle(const QString &title) {
    m_chart->setTitle(title);
}

void BaseChartWidget::setLineColor(const QColor &color) {
    QPen pen = m_series->pen();
    pen.setColor(color);
    m_series->setPen(pen);
}

void BaseChartWidget::setBackgroundColor(const QColor &color) {
    m_chart->setBackgroundBrush(QBrush(color));
}

void BaseChartWidget::setGridVisible(bool visible) {
    m_axisX->setGridLineVisible(visible);
    m_axisY->setGridLineVisible(visible);
}

void BaseChartWidget::resetView() {
    m_axisX->setRange(m_recoverXMin, m_recoverXMax);
    m_axisY->setRange(m_recoverYMin, m_recoverYMax);
}

void BaseChartWidget::optimizeAxisRanges(const QVector<QPointF> &data) {
    double minX = data.first().x();
    double maxX = data.last().x();
    double minY = data.first().y();
    double maxY = data.last().y();
    
    for (const auto &point : data) {
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }
    
    // 添加10%边距
    double yMargin = qMax(0.1, (maxY - minY) * 0.1);
    
    // 记录恢复范围
    m_recoverXMin = minX;
    m_recoverXMax = maxX;
    m_recoverYMin = minY - yMargin;
    m_recoverYMax = maxY + yMargin;
    
    m_axisX->setRange(minX, maxX);
    m_axisY->setRange(minY - yMargin, maxY + yMargin);

}

void BaseChartWidget::setRecoverRange(double xMin, double xMax, double yMin, double yMax) {
    m_recoverXMin = xMin;
    m_recoverXMax = xMax;
    m_recoverYMin = yMin;
    m_recoverYMax = yMax;
}

// 其他实现方法（tooltip, resizeEvent等）与之前的SpectrumChartWidget类似
// 为节省空间，这里省略具体实现，但会包含在最终代码中



void BaseChartWidget::resizeEvent(QResizeEvent *event)
{

    m_chart->resize(event->size());

    const auto callouts = m_callouts;
    for (Callout *callout : callouts)
        callout->updateGeometry();
    

    resize(size());
}

void BaseChartWidget::keepCallout()
{
//    m_callouts.append(m_tooltip);
//    m_tooltip = new Callout(chart);
}

void BaseChartWidget::tooltip(QPointF point, bool state)
{
    if (!m_tooltip)
        m_tooltip = new Callout(m_chart);

    if (state) {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));

        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
        updateCoordText(point);
    } else {
        m_tooltip->hide();
    }
}

void BaseChartWidget::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // Ctrl + 滚轮，调整 X 轴范围
        QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).first());
        if (event->angleDelta().y() > 0) {
            // 放大 X 轴（缩小范围）
            xAxis->setRange(xAxis->min(), xAxis->max() * 0.8);
        } else {
            // 缩小 X 轴（放大范围）
            xAxis->setRange(xAxis->min(), xAxis->max() * 1.25);
        }
    }else{
        // 普通滚轮，调整 Y 轴范围
        QValueAxis *yAxis = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).first());
        if (yAxis) {
            double rangeMin = yAxis->min();
            double rangeMax = yAxis->max();
            double range = rangeMax - rangeMin;

            if (event->angleDelta().y() > 0) {
                // 放大 Y 轴（缩小范围）
                yAxis->setRange(rangeMin, rangeMax * 0.8);
            } else {
                // 缩小 Y 轴（放大范围）
                yAxis->setRange(rangeMin, rangeMax * 1.25);
            }

            // 动态调整主刻度数量
            // range = yAxis->max() - yAxis->min();
            // int tickCount = static_cast<int>(range / 10) + 1; // 根据范围计算主刻度数量
            // yAxis->setTickInterval(10); // 主刻度步长为10
            // yAxis->setTickCount(tickCount); // 设置主刻度数量
        }

    }
    event->accept();
}



void BaseChartWidget::updateCoordText(const QPointF& chartPos) {
    // if(set_freq){
    //     m_coordX->setText(QString("X: %1 MHZ").arg(chartPos.x()));
    //     m_coordY->setText(QString("Y: %1").arg(chartPos.y()));
    // }
    // else{
        m_coordX->setText(QString("X: %1").arg(chartPos.x()));
        m_coordY->setText(QString("Y: %1").arg(chartPos.y()));
    // }
            
    
    // 重新定位到右上角
    QRectF plotArea = m_chart->plotArea();
    qreal right = plotArea.right();
    qreal top = plotArea.top();
    m_coordX->setPos(right - m_coordX->boundingRect().width() - 10, top + 10);
    m_coordY->setPos(right - m_coordY->boundingRect().width() - 10,
                     top + 10 + m_coordX->boundingRect().height() + 5);
}


// todo：需要大幅度修改
void BaseChartWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F) {
        // 弹出输入框，获取用户输入的 X 值
        bool ok;
        double targetX = QInputDialog::getDouble(this, "查找点", "请输入 X 坐标值：", 0, -10000, 10000, 2, &ok);

        if (ok) {
            // 查找对应点
            QPointF closestPoint;
            double minDistance = std::numeric_limits<double>::max();

            for (const QPointF &point : m_series->points()) {
                double distance = std::abs(point.x() - targetX);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestPoint = point;
                }
            }

            // 如果找到点，则标注坐标
            if (minDistance < std::numeric_limits<double>::max()) {
                if (!m_tooltip)
                    m_tooltip = new Callout(m_chart);
                m_tooltip->setAnchor(closestPoint);
                m_tooltip->setZValue(11);
                m_tooltip->updateGeometry();
                m_tooltip->show();
            } else {
                QMessageBox::information(this, "查找点", "未找到对应的点！");
            }
        }
    }

    QWidget::keyPressEvent(event);
}


void BaseChartWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        

        auto scrollDis = m_chartView->getScrollDistance();
        m_chart->scroll(scrollDis.x(), -scrollDis.y()); // 重置滚动位置

        m_chart->zoomReset();
        // 复位缩放
        QValueAxis *axisX = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).constFirst());
        QValueAxis *axisY = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).constFirst());

        if (axisX) axisX->setRange(axisX->min(), m_recoverXMax);
        if (axisY) axisY->setRange(axisY->min(), m_recoverYMax);

        // 重置滚动距离
        m_chartView->resetScrollDistance();

        
    }
    QWidget::mouseDoubleClickEvent(event);
}

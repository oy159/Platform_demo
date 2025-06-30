//
// Created by oycr on 2025/4/4.
//

#include <QVBoxLayout>
#include "SpectrumChartWidget.h"

SpectrumChartWidget::SpectrumChartWidget(QWidget *parent) : QWidget(parent){
    resize(800, 600);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);

    // 创建计数标签
    // counterLabel = new QLabel(this);
    // counterLabel->setAlignment(Qt::AlignCenter);
    // counterLabel->setStyleSheet("QLabel {"
    //                             "background-color: rgba(50, 50, 50, 150);"
    //                             "color: white;"
    //                             "padding: 5px;"
    //                             "border-radius: 5px;"
    //                             "}");
    // updateCounterLabel();  // 初始化标签文本

    // 初始化图表
    initChart();

    setCursor(Qt::ArrowCursor); // 恢复默认指针样式

    // slider = new mSlider(this);
    // slider->setOrientation(Qt::Horizontal);
    // slider->setRange(0, 1000);
    // slider->setTracking(true);
    // slider->setPageStep(1);

    // connect(slider, &QSlider::valueChanged, this, &SpectrumChartWidget::onSliderValueChanged);


    // 将控件添加到布局
    // mainLayout->addWidget(counterLabel);
    mainLayout->addWidget(chartView);
    // mainLayout->addWidget(slider);
}

SpectrumChartWidget::~SpectrumChartWidget() {
    delete chart;
    delete series;
    delete axisX;
    delete axisY;
    delete chartView;
}

void SpectrumChartWidget::handleRefreshPeakData(const std::vector<Peak> &peaks)
{
    mpeaks = peaks;
    qDebug() << "Received peaks:" << mpeaks[0].position << mpeaks[0].value;
    qDebug() << "Received peaks:" << mpeaks[1].position << mpeaks[1].value;

    peakSeries->clear();

    // 添加新的峰值点
    // peakSeries->append((double)peaks[0].position/sampleNum/2*sampleRate/1e6, peaks[0].value);
    // peakSeries->append((double)peaks[1].position/sampleNum/2*sampleRate/1e6, peaks[1].value);

}

void SpectrumChartWidget::handleRefreshChart(QList<QPointF> data) {
    QVector<QPointF> chart_data;
    sampleNum = data.size();
        for (size_t i = 0; i < sampleNum; ++i) {
            chart_data.append(data[i]);
        }
    this->updateWaveformData(chart_data);
}

void SpectrumChartWidget::handleRefreshSpectrum(std::vector<double> fft_data) {
    QVector<QPointF> chart_data;

    sampleNum = fft_data.size();
    if (set_freq){
        for (size_t i = 0; i < fft_data.size(); ++i) {
            chart_data.append(QPointF((double)i/fft_data.size()/2*sampleRate/1e6, fft_data[i]));
        }
    }else{
        for (size_t i = 0; i < fft_data.size(); ++i) {
            chart_data.append(QPointF(i, fft_data[i]));
        }
    }
    this->updateWaveformData(chart_data);
}

void SpectrumChartWidget::handleFindPeak()
{
    peakSeries->clear();
    peakSeries->append((double)mpeaks[0].position/sampleNum/2*sampleRate/1e6, mpeaks[0].value);
    updateCoordText(QPointF((double)mpeaks[0].position/sampleNum/2*sampleRate/1e6, mpeaks[0].value));
    peakCnt = 0;
}

void SpectrumChartWidget::handleFindNextPeak()
{
    peakCnt++;
    if (peakCnt >= mpeaks.size()) {
        peakCnt = 0; // 重置计数器
    }else {
        peakSeries->clear();
        peakSeries->append((double)mpeaks[peakCnt].position/sampleNum/2*sampleRate/1e6, mpeaks[peakCnt].value);
        updateCoordText(QPointF((double)mpeaks[peakCnt].position/sampleNum/2*sampleRate/1e6, mpeaks[peakCnt].value));
    }
}

void SpectrumChartWidget::onSliderValueChanged(int value) {
    if (!axisX) return;

    // 计算新的 X 轴范围
    double range = axisX->max() - axisX->min();
    double newMin = value;
    double newMax = value + range;

    // 更新 X 轴范围
    axisX->setRange(newMin, newMax);
}

void SpectrumChartWidget::initChart() {
    chart = new QChart();
    chart->setAnimationOptions(QChart::NoAnimation); 
    chart->setMargins(QMargins(5, 5, 5, 5));

    series = new QLineSeries();
    series->setUseOpenGL(true); 

    // series->setMarkerSize(16);
    // series->setLightMarker(circle(16, Qt::red));
    // series->setSelectedLightMarker(circle(16, Qt::red));

    QObject::connect(series, &QXYSeries::clicked, series, [=](const QPointF &point) {
        int index = series->points().indexOf(point);
        if (index != -1)
            series->toggleSelection({index});
    });

    QPen pen(Qt::cyan);
    pen.setWidthF(1.5);
    series->setPen(pen);

    chart->addSeries(series);

    // 初始化坐标轴
    axisX = new QValueAxis;
    axisY = new QValueAxis;

    axisX->setTitleText("点数");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(10); // 主刻度数量

    axisY->setTitleText("幅值");
    axisY->setLabelFormat("%d");
    axisY->setTickCount(10);
    axisY->setTickInterval(10); // 设置主刻度步长为10

    // 初始范围

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    peakSeries = new QScatterSeries();
    peakSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle); // 圆形标记
    peakSeries->setMarkerSize(10.0); // 设置标记大小
    peakSeries->setColor(Qt::red); // 设置标记颜色
    chart->addSeries(peakSeries);
    // chart->createDefaultAxes();
    peakSeries->attachAxis(axisX);
    peakSeries->attachAxis(axisY);

    chartView = new NavigationChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    chartView->setRenderHint(QPainter::SmoothPixmapTransform, true);
    chartView->setDragMode(QGraphicsView::RubberBandDrag); // 允许拖动

    // 允许缩放
    chartView->setMouseTracking(true);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
    
    // 隐藏图例
    chart->legend()->hide();

    connect(series, &QLineSeries::clicked, this, &SpectrumChartWidget::keepCallout);
    connect(series, &QLineSeries::hovered, this, &SpectrumChartWidget::tooltip);


    m_coordX = new QGraphicsSimpleTextItem(chart);
    m_coordX->setText("X: ");
    m_coordY = new QGraphicsSimpleTextItem(chart);
    m_coordY->setText("Y: ");

    m_coordX->setBrush(Qt::black);
    m_coordY->setBrush(Qt::black);


    
    // chart->hide();
}

void SpectrumChartWidget::resizeEvent(QResizeEvent *event)
{

    chart->resize(event->size());

    const auto callouts = m_callouts;
    for (Callout *callout : callouts)
        callout->updateGeometry();
    

    resize(size());
}

void SpectrumChartWidget::keepCallout()
{
//    m_callouts.append(m_tooltip);
//    m_tooltip = new Callout(chart);
}

void SpectrumChartWidget::tooltip(QPointF point, bool state)
{
    if (!m_tooltip)
        m_tooltip = new Callout(chart);

    if (state) {
        if(set_freq)
            m_tooltip->setText(QString("X: %1 MHz\nY: %2 ").arg(point.x()).arg(point.y()));
        else
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

void SpectrumChartWidget::updateCoordText(const QPointF& chartPos) {
    if(set_freq){
        m_coordX->setText(QString("X: %1 MHZ").arg(chartPos.x()));
        m_coordY->setText(QString("Y: %1").arg(chartPos.y()));
    }
    else{
        m_coordX->setText(QString("X: %1").arg(chartPos.x()));
        m_coordY->setText(QString("Y: %1").arg(chartPos.y()));
    }
            
    
    // 重新定位到右上角
    QRectF plotArea = chart->plotArea();
    qreal right = plotArea.right();
    qreal top = plotArea.top();
    m_coordX->setPos(right - m_coordX->boundingRect().width() - 10, top + 10);
    m_coordY->setPos(right - m_coordY->boundingRect().width() - 10,
                     top + 10 + m_coordX->boundingRect().height() + 5);
}


void SpectrumChartWidget::updateWaveformData(const QVector<QPointF> &newData)
{
    if (newData.isEmpty()) return;

    // 批量替换数据
    series->clear();
    series->replace(newData);

    // 优化坐标轴范围
    if(firstDataReceived){
        optimizeAxisRanges(newData);
        firstDataReceived = false;
    }
//    optimizeAxisRanges(newData);
}


void SpectrumChartWidget::optimizeAxisRanges(const QVector<QPointF> &data)
{
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

    // 获取现有轴并更新范围
    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).constFirst());
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).constFirst());

    if (axisX) axisX->setRange(minX, maxX);
    if (axisY) axisY->setRange(minY - yMargin, maxY + yMargin);


    recoverX = maxX;
    recoverY = maxY + yMargin;

    // slider->setValue(data.first().x());
    // qDebug() << "Slider range:" << slider->value();
}

QImage SpectrumChartWidget::circle(qreal imageSize, const QColor &color)
{
    QImage image(imageSize, imageSize, QImage::Format_ARGB32);
    image.fill(QColor(0, 0, 0, 0));
    QPainter paint;
    paint.begin(&image);
    paint.setBrush(color);
    QPen pen = paint.pen();
    pen.setWidth(0);
    paint.setPen(pen);
    paint.drawEllipse(0, 0, imageSize * 0.9, imageSize * 0.9);
    paint.end();
    return image;
}


void SpectrumChartWidget::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // Ctrl + 滚轮，调整 X 轴范围
        QValueAxis *xAxis = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
        if (event->angleDelta().y() > 0) {
            // 放大 X 轴（缩小范围）
            xAxis->setRange(xAxis->min(), xAxis->max() * 0.8);
        } else {
            // 缩小 X 轴（放大范围）
            xAxis->setRange(xAxis->min(), xAxis->max() * 1.25);
        }
    }else{
        // 普通滚轮，调整 Y 轴范围
        QValueAxis *yAxis = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
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



// todo：需要大幅度修改
void SpectrumChartWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F) {
        // 弹出输入框，获取用户输入的 X 值
        bool ok;
        double targetX = QInputDialog::getDouble(this, "查找点", "请输入 X 坐标值：", 0, -10000, 10000, 2, &ok);

        if (ok) {
            // 查找对应点
            QPointF closestPoint;
            double minDistance = std::numeric_limits<double>::max();

            for (const QPointF &point : series->points()) {
                double distance = std::abs(point.x() - targetX);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestPoint = point;
                }
            }

            // 如果找到点，则标注坐标
            if (minDistance < std::numeric_limits<double>::max()) {
                if (!m_tooltip)
                    m_tooltip = new Callout(chart);
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


void SpectrumChartWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        

        auto scrollDis = chartView->getScrollDistance();
        chart->scroll(scrollDis.x(), -scrollDis.y()); // 重置滚动位置

        chart->zoomReset();
        // 复位缩放
        QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).constFirst());
        QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).constFirst());

        if (axisX) axisX->setRange(axisX->min(), recoverX);
        if (axisY) axisY->setRange(axisY->min(), recoverY);

        // 重置滚动距离
        chartView->resetScrollDistance();

        
    }
    QWidget::mouseDoubleClickEvent(event);
}


void SpectrumChartWidget::adjustaxisX(){
    axisX->setLabelFormat("%.2f");
}
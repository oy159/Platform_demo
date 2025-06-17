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

    slider = new mSlider(this);
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(0, 1000);
    slider->setTracking(true);
    slider->setPageStep(1);

    connect(slider, &QSlider::valueChanged, this, &SpectrumChartWidget::onSliderValueChanged);


    // 将控件添加到布局
    // mainLayout->addWidget(counterLabel);
    mainLayout->addWidget(chartView);
    mainLayout->addWidget(slider);
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
    peakSeries->append(peaks[0].position, peaks[0].value);
    peakSeries->append(peaks[1].position, peaks[1].value);

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

    QPen pen(Qt::cyan);
    pen.setWidthF(1.5);
    series->setPen(pen);

    chart->addSeries(series);

    // 初始化坐标轴
    axisX = new QValueAxis;
    axisY = new QValueAxis;

    axisX->setTitleText("点数");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(11); // 主刻度数量

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
    peakSeries->attachAxis(axisX);
    peakSeries->attachAxis(axisY);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    chartView->setRenderHint(QPainter::SmoothPixmapTransform, true);
    chartView->setDragMode(QGraphicsView::ScrollHandDrag); // 允许拖动

    // 允许缩放
    chartView->setMouseTracking(true);

    // 隐藏图例
    chart->legend()->hide();
}

void SpectrumChartWidget::updateCounterLabel() {
    // counterLabel->setText(QString("频谱图：第 %1 张").arg(updateCount));
}

void SpectrumChartWidget::updateWaveformData(const QVector<QPointF> &newData)
{
    if (newData.isEmpty()) return;

    // 批量替换数据
    series->clear();
    series->replace(newData);
    
    slider->setRange(newData.first().x(), newData.last().x());
    

    updateCount++;
    updateCounterLabel();

    // 优化坐标轴范围
    if(firstDataReceived){
        optimizeAxisRanges(newData);
        firstDataReceived = false;
    }
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

    slider->setValue(data.first().x());
    qDebug() << "Slider range:" << slider->value();
}

void SpectrumChartWidget::AdjustAxisXRange(double min, double max)
{
    // 获取现有轴并更新范围
    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).constFirst());
    slider->setValue(min);

    if (axisX) axisX->setRange(min, max);
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
    } else if(event->modifiers() & Qt::ShiftModifier) {
        QValueAxis *xAxis = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
        int step = (xAxis->max() - xAxis->min()) / 100; // 每次移动1%
        // 调整 X 轴范围
        if (event->angleDelta().y() > 0) {
            // 往右移动
            xAxis->setRange(xAxis->min() + step, xAxis->max() + step);
        } else {
            // 往左移动
            xAxis->setRange(xAxis->min() - step, xAxis->max() - step);
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
            range = yAxis->max() - yAxis->min();
            int tickCount = static_cast<int>(range / 10) + 1; // 根据范围计算主刻度数量
            yAxis->setTickInterval(10); // 主刻度步长为10
            yAxis->setTickCount(tickCount); // 设置主刻度数量
        }

    }
    event->accept();
}

void SpectrumChartWidget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F) {
        // 弹出输入框，获取用户输入的 X 值
        bool ok;
        double targetX = QInputDialog::getInt(this, "查找点", "请输入 X 坐标值：", 0, -10000, 10000, 2, &ok);

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
                showPointCoordinates(closestPoint);
            } else {
                QMessageBox::information(this, "查找点", "未找到对应的点！");
            }
        }
    }

    QWidget::keyPressEvent(event);
}

void SpectrumChartWidget::showPointCoordinates(const QPointF &point) {
    // 删除之前的坐标显示
    if (coordinateLabel) {
        delete coordinateLabel;
        coordinateLabel = nullptr;
    }

    // 创建一个 QLabel 显示坐标
    coordinateLabel = new QLabel(this);
    coordinateLabel->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
    coordinateLabel->setStyleSheet("QLabel { background-color: white; color: black; padding: 3px; }");

    // 将坐标转换为图表上的位置
    QPoint labelPos = chartView->mapFromScene(chart->mapToPosition(point, series));
    coordinateLabel->move(labelPos.x(), labelPos.y() - 20); // 调整位置以避免遮挡点
    coordinateLabel->show();
}

void SpectrumChartWidget::handleRefreshSpectrum(std::vector<double> fft_data) {
    QVector<QPointF> chart_data;
    for (size_t i = 0; i < fft_data.size(); ++i) {
        chart_data.append(QPointF(i, fft_data[i]));
    }
    this->updateWaveformData(chart_data);
}

mSlider::mSlider(QWidget *parent)
{

}

mSlider::~mSlider()
{
}

void mSlider::mousePressEvent(QMouseEvent *ev)
{
     //获取当前点击位置
     int currentX = ev->pos().x();
 
     //获取当前点击的位置占整个Slider的百分比
     double per = currentX *1.0 /this->width();
  
     //利用算得的百分比得到具体数字
     int value = per*(this->maximum() - this->minimum()) + this->minimum();
  
     //设定滑动条位置
     this->setValue(value);
  
     //滑动条移动事件等事件也用到了mousePressEvent
     QSlider::mousePressEvent(ev);
}

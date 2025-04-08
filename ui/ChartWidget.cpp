//
// Created by oycr on 2025/4/4.
//

#include <QVBoxLayout>
#include "ChartWidget.h"

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent){
    resize(800, 600);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);

    // 创建计数标签
    counterLabel = new QLabel(this);
    counterLabel->setAlignment(Qt::AlignCenter);
    counterLabel->setStyleSheet("QLabel {"
                                "background-color: rgba(50, 50, 50, 150);"
                                "color: white;"
                                "padding: 5px;"
                                "border-radius: 5px;"
                                "}");
    updateCounterLabel();  // 初始化标签文本

    // 初始化图表
    initChart();

    auto* slider = new mSlider(this);
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(0, 1000);
    slider->setTracking(true);
    slider->setPageStep(1);

    // slider->setStyleSheet(
    //     "QSlider::groove:horizontal { "
    //     "    height: 12px; "
    //     "    left: 0px; "
    //     "    right: 0px; "
    //     "    border: 0px; "
    //     "    border-radius: 6px; "
    //     "    background: rgb(34, 173, 143); "
    //     "} "

    //     "QSlider::handle:horizontal { "
    //     "    width: 50px; "
    //     "    height: 50px; "
    //     "    margin-top: -20px; "
    //     "    margin-left: 0px; "
    //     "    margin-bottom: -20px; "
    //     "    margin-right: 0px; "
    //     "    border-image: url(:/res/images/setting_slider_handle.png); "
    //     "} "
    //     "QSlider::sub-page:horizontal { "
    //     "    background: rgb(48, 84, 112); "
    //     "}"
    // );



    // 将控件添加到布局
    mainLayout->addWidget(counterLabel);
    mainLayout->addWidget(chartView);
    mainLayout->addWidget(slider);
}

ChartWidget::~ChartWidget() {
    delete chart;
    delete series;
    delete axisX;
    delete axisY;
    delete chartView;
}


void ChartWidget::initChart() {
    chart = new QChart();
    chart->setAnimationOptions(QChart::NoAnimation); // 禁用动画提高性能
    chart->setMargins(QMargins(5, 5, 5, 5));

    series = new QLineSeries();
    series->setUseOpenGL(true); // 必须开启OpenGL加速

    // 配置抗锯齿和线条样式
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
    axisY->setTickCount(20);

    // 初始范围
    axisX->setRange(0, 100);
    axisY->setRange(-1.5, 1.5);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    chartView->setRenderHint(QPainter::SmoothPixmapTransform, true);

    chartView->setDragMode(QGraphicsView::ScrollHandDrag); // 允许拖动
    chartView->setRenderHint(QPainter::Antialiasing, true);
    chartView->setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 允许缩放
    chartView->setMouseTracking(true);

    // 隐藏图例
    chart->legend()->hide();
}

void ChartWidget::updateCounterLabel() {
    counterLabel->setText(QString("频谱图：第 %1 张").arg(updateCount));
}

void ChartWidget::updateWaveformData(const QVector<QPointF> &newData)
{
    if (newData.isEmpty()) return;

    // 批量替换数据
    series->clear();
    series->replace(newData);

    updateCount++;
    updateCounterLabel();

    // 优化坐标轴范围
    if(firstDataReceived){
        optimizeAxisRanges(newData);
        firstDataReceived = false;
    }
}



void ChartWidget::optimizeAxisRanges(const QVector<QPointF> &data)
{
    double minX = data.first().x();
    double maxX = data.last().x();
    double minY = data.first().y();
    double maxY = data.first().y();

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
}

void ChartWidget::AdjustAxisXRange(double min, double max)
{
    // 获取现有轴并更新范围
    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).constFirst());

    if (axisX) axisX->setRange(min, max);
}

void ChartWidget::wheelEvent(QWheelEvent *event) {
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
        if (event->angleDelta().y() > 0) {
            // 放大 Y 轴（缩小范围）
            yAxis->setRange(yAxis->min() * 1.2, yAxis->max() * 1.2);
        } else {
            // 缩小 Y 轴（放大范围）
            yAxis->setRange(yAxis->min() * 0.8, yAxis->max() * 0.8);
        }

    }
    event->accept();
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
  
     //滑动条移动事件等事件也用到了mousePressEvent,加这句话是为了不对其产生影响，是的Slider能正常相应其他鼠标事件
     QSlider::mousePressEvent(ev);
}

//
// Created by oycr on 2025/4/4.
//

#include <QVBoxLayout>
#include "ChartWidget.h"

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent){
    resize(1024, 600);
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

    // 将控件添加到布局
    mainLayout->addWidget(counterLabel);
    mainLayout->addWidget(chartView);
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
    } else {
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


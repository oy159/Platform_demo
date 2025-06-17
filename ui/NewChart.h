#ifndef NEWCHART_H
#define NEWCHART_H

#include <QWidget>
#include <QGridLayout>
#include <QtCharts>
#include <QGraphicsSceneMouseEvent>
#include <QRandomGenerator>

class MultiChartGridWidget : public QWidget {
    Q_OBJECT
public:
    MultiChartGridWidget(QWidget *parent = nullptr) : QWidget(parent) {
        gridLayout = new QGridLayout(this);
        
        // 初始添加一个图表
        // addChart();
        
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QWidget::customContextMenuRequested, 
                this, &MultiChartGridWidget::showContextMenu);
    }

public slots:
    void addChart() {
        QChart *chart = new QChart();
        QChartView *chartView = new QChartView(chart);
        chart->setAnimationOptions(QChart::NoAnimation); 
        
        // 简单示例数据
        QLineSeries *series = new QLineSeries();
        for (int i = 0; i < 10; ++i)
            series->append(i, QRandomGenerator::global()->generate() % 10);
        
        series->setUseOpenGL(true); 
        chart->addSeries(series);
        chart->createDefaultAxes();
        chart->setTitle(QString("Chart %1").arg(chartViews.size() + 1));
        
        chartViews.append(chartView);
        reorganizeCharts();
        chart->legend()->hide();
    }

private slots:
    void showContextMenu(const QPoint &pos) {
        QMenu menu(this);
        menu.addAction("Add Chart", this, &MultiChartGridWidget::addChart);
        menu.exec(mapToGlobal(pos));
    }

private:
    void reorganizeCharts() {
        // 清除现有布局
        QLayoutItem *child;
        while ((child = gridLayout->takeAt(0)) != nullptr) {
            delete child;
        }
        
        // 计算最佳的行列数
        int count = chartViews.size();
        int rows = qCeil(qSqrt(count));
        int cols = qCeil(count / double(rows));
        
        // 重新排列图表
        for (int i = 0; i < count; ++i) {
            int row = i / cols;
            int col = i % cols;
            gridLayout->addWidget(chartViews[i], row, col);
        }
        
        // 设置拉伸因子使图表均匀分布
        for (int r = 0; r < rows; ++r)
            gridLayout->setRowStretch(r, 1);
        for (int c = 0; c < cols; ++c)
            gridLayout->setColumnStretch(c, 1);
    }

    QGridLayout *gridLayout;
    QList<QChartView*> chartViews;
};


class NewChart : public MultiChartGridWidget {
    Q_OBJECT

public:
    NewChart(QWidget *parent = nullptr) : MultiChartGridWidget(parent) {
        setWindowTitle("Multi Chart Grid");
        setMinimumSize(800, 600);
    }

    ~NewChart() override = default;

};


class SpectrumMarker : public QGraphicsItem {
public:
    explicit SpectrumMarker(QChart *chart) : m_chart(chart) {
        setFlags(QGraphicsItem::ItemIsMovable | 
                QGraphicsItem::ItemSendsScenePositionChanges |
                QGraphicsItem::ItemIsSelectable);
        setAcceptHoverEvents(true);
    }

    QRectF boundingRect() const override {
        return QRectF(-10, -10, 20, 20);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override {
        // 绘制Marker中心点
        painter->setPen(QPen(Qt::red, 2));
        painter->setBrush(Qt::red);
        painter->drawEllipse(-5, -5, 10, 10);
        
        // 绘制垂直线
        QLineF line(0, -1000, 0, 1000); // 足够长的线
        painter->drawLine(line);
    }

    void updatePosition(qreal xValue) {
        QPointF newPos = m_chart->mapToPosition(QPointF(xValue, 0));
        setPos(newPos);
    }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        if (change == ItemPositionChange && scene()) {
            // 限制只能在X轴方向移动
            QPointF newPos = value.toPointF();
            QPointF chartPos = m_chart->mapToValue(newPos);
            newPos.setX(m_chart->mapToPosition(QPointF(chartPos.x(), 0)).x());
            return newPos;
        }
        return QGraphicsItem::itemChange(change, value);
    }

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override {
        QGraphicsItem::hoverMoveEvent(event);
        update();
    }

private:
    QChart *m_chart;
};

class SpectrumChartView : public QChartView {
    Q_OBJECT
public:
    explicit SpectrumChartView(QWidget *parent = nullptr) : QChartView(parent) {
        setRenderHint(QPainter::Antialiasing);
        setMouseTracking(true);
        
        // 创建图表和数据
        initChart();
        
        // 创建Marker
        m_marker = new SpectrumMarker(chart());
        chart()->scene()->addItem(m_marker);
        m_marker->updatePosition(50); // 初始位置
        
        // 创建数据显示标签
        m_valueLabel = new QGraphicsSimpleTextItem(chart());
        m_valueLabel->setBrush(QBrush(Qt::red));
        m_valueLabel->setFont(QFont("Arial", 10));
        updateValueLabel(50);
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        if (m_dragging) {
            // 拖动Marker时更新位置
            QPointF chartPos = chart()->mapToValue(event->pos());
            m_marker->updatePosition(chartPos.x());
            updateValueLabel(chartPos.x());
        }
        QChartView::mouseMoveEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            // 检查是否点击了Marker
            QPointF scenePos = mapToScene(event->pos());
            if (m_marker->contains(m_marker->mapFromScene(scenePos))) {
                m_dragging = true;
                return;
            }
        }
        QChartView::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragging = false;
        }
        QChartView::mouseReleaseEvent(event);
    }

private:
    void initChart() {
        QChart *c = new QChart();
        QLineSeries *series = new QLineSeries();
        
        // 生成示例频谱数据
        for (int i = 0; i < 100; ++i) {
            qreal y = qSin(i / 10.0) * 50 + QRandomGenerator::global()->bounded(20);
            series->append(i, y);
        }
        
        c->addSeries(series);
        c->createDefaultAxes();
        c->setTitle("频谱分析 (拖动红色标记线)");
        setChart(c);
    }

    void updateValueLabel(qreal xValue) {
        // 找到最近的数据点
        QLineSeries *series = qobject_cast<QLineSeries*>(chart()->series().at(0));
        QPointF closestPoint;
        qreal minDist = std::numeric_limits<qreal>::max();
        
        foreach (const QPointF &point, series->points()) {
            qreal dist = qAbs(point.x() - xValue);
            if (dist < minDist) {
                minDist = dist;
                closestPoint = point;
            }
        }
        
        // 更新标签文本和位置
        QString text = QString("X: %1\nY: %2 dB").arg(closestPoint.x(), 0, 'f', 1)
                                               .arg(closestPoint.y(), 0, 'f', 1);
        m_valueLabel->setText(text);
        
        QPointF labelPos = chart()->mapToPosition(closestPoint);
        m_valueLabel->setPos(labelPos + QPointF(15, -30));
    }

    SpectrumMarker *m_marker;
    QGraphicsSimpleTextItem *m_valueLabel;
    bool m_dragging = false;
};


#endif
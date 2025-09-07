#ifndef CURSOR_MARKER_CURSOR_H
#define CURSOR_MARKER_CURSOR_H

#include <QChart>
#include <QFont>
#include <QGraphicsItem>

QT_FORWARD_DECLARE_CLASS(QChart)
QT_FORWARD_DECLARE_CLASS(QGraphicsSceneMouseEvent)

class Cursor : public QGraphicsItem {
public:
    explicit Cursor(QChart *parent);
    ~Cursor() override = default;

    // QGraphicsItem接口
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // 鼠标事件处理
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    // 光标线控制方法
    void setVerticalLine1Pos(qreal x);
    void setVerticalLine2Pos(qreal x);
    void setHorizontalLine1Pos(qreal y);
    void setHorizontalLine2Pos(qreal y);

    qreal verticalLine1Pos() const;
    qreal verticalLine2Pos() const;
    qreal horizontalLine1Pos() const;
    qreal horizontalLine2Pos() const;

    // 样式设置
    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setLineStyle(Qt::PenStyle style);
    void setFont(const QFont &font);

    // 可见性控制
    void setVerticalLinesVisible(bool visible);
    void setHorizontalLinesVisible(bool visible);
    void setTextVisible(bool visible);

    // 更新位置（当图表大小变化时调用）
    void updateGeometry();

private:
    // 计算测量结果
    void calculateMeasurements();

    // 检测鼠标点击的是哪条线
    int hitTest(const QPointF &pos);

    QChart *mChart;

    // 线条位置（使用图表数据坐标）
    qreal m_vLine1Pos;
    qreal m_vLine2Pos;
    qreal m_hLine1Pos;
    qreal m_hLine2Pos;

    // 样式属性
    QColor m_lineColor;
    QColor m_textColor;
    Qt::PenStyle m_lineStyle;
    QFont m_font;

    // 可见性控制
    bool m_verticalLinesVisible;
    bool m_horizontalLinesVisible;
    bool m_textVisible;

    // 交互状态
    bool m_dragging;
    int m_draggingLine; // 0:无, 1:垂直线1, 2:垂直线2, 3:水平线1, 4:水平线2
    QPointF m_dragStartPos;

    // 测量结果
    qreal m_deltaX;
    qreal m_deltaY;
    qreal m_frequency;


    qreal m_triangleSize;

    // 文本显示区域
    QRectF m_textRect;
};

#endif // CURSOR_MARKER_CURSOR_H
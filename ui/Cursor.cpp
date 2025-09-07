#include "cursor.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

Cursor::Cursor(QChart *parent)
    : QGraphicsItem(parent)
      , mChart(parent)
      , m_vLine1Pos(-1)
      , m_vLine2Pos(-1)
      , m_hLine1Pos(-1)
      , m_hLine2Pos(-1)
      , m_lineColor(Qt::red)
      , m_textColor(Qt::black)
      , m_lineStyle(Qt::DashLine)
      , m_font("Arial", 10)
      , m_verticalLinesVisible(true)
      , m_horizontalLinesVisible(true)
      , m_textVisible(true)
      , m_dragging(false)
      , m_draggingLine(0)
      , m_deltaX(0)
      , m_deltaY(0)
      , m_frequency(0)
      , m_triangleSize(12.0) {
    setAcceptHoverEvents(true);
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

QRectF Cursor::boundingRect() const {
    if (!mChart)
        return QRectF();

    QRectF plotArea = mChart->plotArea();
    // 扩大边界矩形以包含可能的文本
    return plotArea.adjusted(-100, -100, 100, 100);
}

void Cursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!mChart)
        return;

    painter->setRenderHint(QPainter::Antialiasing);

    // 获取图表绘图区域
    QRectF plotArea = mChart->plotArea();
    QPointF topLeft = plotArea.topLeft();
    QPointF bottomRight = plotArea.bottomRight();

    // 设置线条样式
    QPen pen(m_lineColor);
    pen.setStyle(m_lineStyle);
    pen.setWidthF(2.1);
    painter->setPen(pen);

    // 绘制垂直线
    if (m_verticalLinesVisible) {
        QPointF vLine1ChartPos = mChart->mapToPosition(QPointF(m_vLine1Pos, 0));

        // 绘制垂直线
        painter->drawLine(QPointF(vLine1ChartPos.x(), topLeft.y()),
                          QPointF(vLine1ChartPos.x(), bottomRight.y()));

        // 在顶部绘制显示X坐标的文本框
        QString xText = QString("%1").arg(m_vLine1Pos, 0, 'f', 2);
        QFontMetrics metrics(m_font);
        QRectF textRect = metrics.boundingRect(xText);
        textRect.moveTo(vLine1ChartPos.x() - textRect.width() / 2, topLeft.y() - textRect.height() - 5);

        // 绘制文本框背景
        painter->save();
        painter->setBrush(QColor(255, 255, 255, 200));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);
        painter->restore();

        // 绘制文本
        painter->save();
        painter->setFont(m_font);
        painter->setPen(m_textColor);
        painter->drawText(textRect, Qt::AlignCenter, xText);
        painter->restore();


        QPointF vLine2ChartPos = mChart->mapToPosition(QPointF(m_vLine2Pos, 0));

        // 绘制垂直线
        painter->drawLine(QPointF(vLine2ChartPos.x(), topLeft.y()),
                          QPointF(vLine2ChartPos.x(), bottomRight.y()));

        // 在顶部绘制显示X坐标的文本框
        xText = QString("%1").arg(m_vLine2Pos, 0, 'f', 2);
        textRect = metrics.boundingRect(xText);
        textRect.moveTo(vLine2ChartPos.x() - textRect.width() / 2, topLeft.y() - textRect.height() - 5);

        // 绘制文本框背景
        painter->save();
        painter->setBrush(QColor(255, 255, 255, 200));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);
        painter->restore();

        // 绘制文本
        painter->save();
        painter->setFont(m_font);
        painter->setPen(m_textColor);
        painter->drawText(textRect, Qt::AlignCenter, xText);
        painter->restore();
    }

    // 绘制水平线
    if (m_horizontalLinesVisible) {
        QPointF hLine1ChartPos = mChart->mapToPosition(QPointF(0, m_hLine1Pos));

        // 绘制水平线
        painter->drawLine(QPointF(topLeft.x(), hLine1ChartPos.y()),
                          QPointF(bottomRight.x(), hLine1ChartPos.y()));

        // 在右端绘制显示Y坐标的文本框
        QString yText = QString("%1").arg(m_hLine1Pos, 0, 'f', 2);
        QFontMetrics metrics(m_font);
        QRectF textRect = metrics.boundingRect(yText);
        textRect.moveTo(bottomRight.x() + 5, hLine1ChartPos.y() - textRect.height() / 2);

        // 绘制文本框背景
        painter->save();
        painter->setBrush(QColor(255, 255, 255, 200));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);
        painter->restore();

        // 绘制文本
        painter->save();
        painter->setFont(m_font);
        painter->setPen(m_textColor);
        painter->drawText(textRect, Qt::AlignCenter, yText);
        painter->restore();


        QPointF hLine2ChartPos = mChart->mapToPosition(QPointF(0, m_hLine2Pos));

        // 绘制水平线
        painter->drawLine(QPointF(topLeft.x(), hLine2ChartPos.y()),
                          QPointF(bottomRight.x(), hLine2ChartPos.y()));

        // 在右端绘制显示Y坐标的文本框
        yText = QString("%1").arg(m_hLine2Pos, 0, 'f', 2);
        textRect = metrics.boundingRect(yText);
        textRect.moveTo(bottomRight.x() + 5, hLine2ChartPos.y() - textRect.height() / 2);

        // 绘制文本框背景
        painter->save();
        painter->setBrush(QColor(255, 255, 255, 200));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);
        painter->restore();

        // 绘制文本
        painter->save();
        painter->setFont(m_font);
        painter->setPen(m_textColor);
        painter->drawText(textRect, Qt::AlignCenter, yText);
        painter->restore();
    }

    // 绘制测量结果文本
    if (m_textVisible) {
        painter->setFont(m_font);
        painter->setPen(m_textColor);

        QString infoText;

        infoText += QString("ΔX: %1\t").arg(m_deltaX, 0, 'f', 2);
        infoText += QString("ΔY: %1").arg(m_deltaY, 0, 'f', 2);


        if (!infoText.isEmpty()) {
            // 计算文本位置，中间位置
            auto textPos = QPointF(bottomRight.x() / 2 - 20, topLeft.y() + 20);
            // 绘制文本背景
            QFontMetrics metrics(m_font);
            QRectF textRect = metrics.boundingRect(infoText);
            textRect.moveTo(textPos);
            textRect.adjust(-2, -2, 2, 2);

            painter->save();
            painter->setBrush(QColor(255, 255, 255, 200)); // 半透明白色背景
            painter->setPen(Qt::NoPen);
            painter->drawRect(textRect);
            painter->restore();

            // 绘制文本
            painter->drawText(textPos, infoText);
        }
    }
}

void Cursor::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    // 检测点击的是哪条线
    m_draggingLine = hitTest(event->pos());
    m_dragging = (m_draggingLine > 0);
    m_dragStartPos = event->pos();

    event->setAccepted(m_dragging);
}

void Cursor::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!m_dragging) {
        event->ignore();
        return;
    }

    // 将鼠标位置转换为图表数据坐标
    QPointF chartPos = mChart->mapToValue(event->pos());
    QPointF startChartPos = mChart->mapToValue(m_dragStartPos);

    // 更新被拖动的线
    switch (m_draggingLine) {
        case 1:
            setVerticalLine1Pos(chartPos.x());
            break;
        case 2:
            setVerticalLine2Pos(chartPos.x());
            break;
        case 3:
            setHorizontalLine1Pos(chartPos.y());
            break;
        case 4:
            setHorizontalLine2Pos(chartPos.y());
            break;
    }

    m_dragStartPos = event->pos();
    event->setAccepted(true);
}

void Cursor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    m_dragging = false;
    m_draggingLine = 0;
    event->setAccepted(true);
}

int Cursor::hitTest(const QPointF &pos) {
    const qreal hitThreshold = 8; // 点击检测阈值（像素）
    // 获取图表绘图区域
    QRectF plotArea = mChart->plotArea();
    QPointF topLeft = plotArea.topLeft();
    QPointF bottomRight = plotArea.bottomRight();

    // 检查垂直线和它们的三角形区域
    if (m_verticalLinesVisible) {

        QPointF vLine1ChartPos = mChart->mapToPosition(QPointF(m_vLine1Pos, 0));

        // 检查是否点击了垂直线
        if (qAbs(pos.x() - vLine1ChartPos.x()) < hitThreshold &&
            pos.y() >= topLeft.y() && pos.y() <= bottomRight.y()) {
            return 1;
        }

        // 检查是否点击了垂直线的顶部三角形
        QRectF triangleRect(
            vLine1ChartPos.x() - m_triangleSize / 2,
            topLeft.y() - m_triangleSize / 2,
            1.2 * m_triangleSize,
            1.2 * m_triangleSize
        );

        if (triangleRect.contains(pos)) {
            return 1;
        }



        QPointF vLine2ChartPos = mChart->mapToPosition(QPointF(m_vLine2Pos, 0));

        // 检查是否点击了垂直线
        if (qAbs(pos.x() - vLine2ChartPos.x()) < hitThreshold &&
            pos.y() >= topLeft.y() && pos.y() <= bottomRight.y()) {
            return 2;
        }

        // 检查是否点击了垂直线的顶部三角形
        QRectF triangleRect2(
            vLine2ChartPos.x() - m_triangleSize / 2,
            topLeft.y(),
            m_triangleSize,
            m_triangleSize
        );

        if (triangleRect2.contains(pos)) {
            return 2;
        }

    }

    // 检查水平线和它们的三角形区域
    if (m_horizontalLinesVisible) {

        QPointF hLine1ChartPos = mChart->mapToPosition(QPointF(0, m_hLine1Pos));

        // 检查是否点击了水平线
        if (qAbs(pos.y() - hLine1ChartPos.y()) < hitThreshold &&
            pos.x() >= topLeft.x() && pos.x() <= bottomRight.x()) {
            return 3;
        }

        // 检查是否点击了水平线的右端三角形
        QRectF triangleRect3(
            bottomRight.x() - m_triangleSize,
            hLine1ChartPos.y() - m_triangleSize / 2,
            m_triangleSize,
            m_triangleSize
        );

        if (triangleRect3.contains(pos)) {
            return 3;
        }



        QPointF hLine2ChartPos = mChart->mapToPosition(QPointF(0, m_hLine2Pos));

        // 检查是否点击了水平线
        if (qAbs(pos.y() - hLine2ChartPos.y()) < hitThreshold &&
            pos.x() >= topLeft.x() && pos.x() <= bottomRight.x()) {
            return 4;
        }

        // 检查是否点击了水平线的右端三角形
        QRectF triangleRect4(
            bottomRight.x() - m_triangleSize,
            hLine2ChartPos.y() - m_triangleSize / 2,
            m_triangleSize,
            m_triangleSize
        );

        if (triangleRect4.contains(pos)) {
            return 4;
        }

    }

    return 0; // 没有点击到任何线或三角形
}

void Cursor::setVerticalLine1Pos(qreal x) {
    m_vLine1Pos = x;
    calculateMeasurements();
    update();
}

void Cursor::setVerticalLine2Pos(qreal x) {
    m_vLine2Pos = x;
    calculateMeasurements();
    update();
}

void Cursor::setHorizontalLine1Pos(qreal y) {
    m_hLine1Pos = y;
    calculateMeasurements();
    update();
}

void Cursor::setHorizontalLine2Pos(qreal y) {
    m_hLine2Pos = y;
    calculateMeasurements();
    update();
}

qreal Cursor::verticalLine1Pos() const {
    return m_vLine1Pos;
}

qreal Cursor::verticalLine2Pos() const {
    return m_vLine2Pos;
}

qreal Cursor::horizontalLine1Pos() const {
    return m_hLine1Pos;
}

qreal Cursor::horizontalLine2Pos() const {
    return m_hLine2Pos;
}

void Cursor::setLineColor(const QColor &color) {
    m_lineColor = color;
    update();
}

void Cursor::setTextColor(const QColor &color) {
    m_textColor = color;
    update();
}

void Cursor::setLineStyle(Qt::PenStyle style) {
    m_lineStyle = style;
    update();
}

void Cursor::setFont(const QFont &font) {
    m_font = font;
    update();
}

void Cursor::setVerticalLinesVisible(bool visible) {
    m_verticalLinesVisible = visible;
    update();
}

void Cursor::setHorizontalLinesVisible(bool visible) {
    m_horizontalLinesVisible = visible;
    update();
}

void Cursor::setTextVisible(bool visible) {
    m_textVisible = visible;
    update();
}

void Cursor::updateGeometry() {
    prepareGeometryChange();
    update();
}

void Cursor::calculateMeasurements() {

    m_deltaX = qAbs(m_vLine2Pos - m_vLine1Pos);
    if (m_deltaX != 0) {
        m_frequency = 1.0 / m_deltaX;
    } else {
        m_frequency = 0;
    }
    m_deltaY = qAbs(m_hLine2Pos - m_hLine1Pos);
}
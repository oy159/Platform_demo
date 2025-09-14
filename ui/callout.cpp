
#include "callout.h"

#include <QChart>
#include <QValueAxis>
#include <qgraphicsscene.h>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QPainter>

Callout::Callout(QChart *chart)
    : QGraphicsItem(chart)
    , m_chart(chart), IfDrawMarker(false)
{
}

Callout::~Callout() {
    if (scene()) {
        scene()->removeItem(this);
    }
}

bool Callout::isAnchorVisible() const {
    if (!m_chart || m_anchor.isNull()) {
        return false;
    }

    // 获取当前坐标轴范围
    QValueAxis *axisX = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).first());

    if (!axisX || !axisY) {
        return false;
    }

    // 检查锚点是否在当前坐标轴范围内
    double xMin = axisX->min();
    double xMax = axisX->max();
    double yMin = axisY->min();
    double yMax = axisY->max();

    return (m_anchor.x() >= xMin && m_anchor.x() <= xMax &&
            m_anchor.y() >= yMin && m_anchor.y() <= yMax);
}


QRectF Callout::boundingRect() const
{
    if (!isAnchorVisible()) {
        // 如果锚点不可见，只返回文本框的矩形
        return m_rect;
    }
    QPointF anchor = mapFromParent(m_chart->mapToPosition(m_anchor));
    QRectF rect;
    rect.setLeft(qMin(m_rect.left(), anchor.x()));
    rect.setRight(qMax(m_rect.right(), anchor.x()));
    rect.setTop(qMin(m_rect.top(), anchor.y()));
    rect.setBottom(qMax(m_rect.bottom(), anchor.y()));
    return rect;
}

void Callout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (!m_chart || m_text.isEmpty()) {
        return;
    }
    QPainterPath path;
    path.addRoundedRect(m_rect, 5, 5);
    const qreal size = 8.0;
    qreal halfSide = size / 1.732;

    if (isAnchorVisible()) {
        QPointF anchor = mapFromParent(m_chart->mapToPosition(m_anchor));
        QPointF point3 = anchor;
        if (IfDrawMarker) {
            QPolygonF triangle;
            triangle << QPointF(anchor.x(), anchor.y())          // 上顶点
                     << QPointF(anchor.x() - halfSide, anchor.y() - size)  // 左上顶点
                     << QPointF(anchor.x() + halfSide, anchor.y() - size); // 右上顶点

            painter->setBrush(m_color);
            painter->setPen(Qt::NoPen);
            painter->drawPolygon(triangle);
            point3 = {anchor.x(), anchor.y() - size};
        }

        if (!m_rect.contains(anchor) && !m_anchor.isNull()) {
            QPointF point1, point2;

            // establish the position of the anchor point in relation to m_rect
            bool above = anchor.y() <= m_rect.top();
            bool aboveCenter = anchor.y() > m_rect.top() && anchor.y() <= m_rect.center().y();
            bool belowCenter = anchor.y() > m_rect.center().y() && anchor.y() <= m_rect.bottom();
            bool below = anchor.y() > m_rect.bottom();

            bool onLeft = anchor.x() <= m_rect.left();
            bool leftOfCenter = anchor.x() > m_rect.left() && anchor.x() <= m_rect.center().x();
            bool rightOfCenter = anchor.x() > m_rect.center().x() && anchor.x() <= m_rect.right();
            bool onRight = anchor.x() > m_rect.right();

            // get the nearest m_rect corner.
            qreal x = (onRight + rightOfCenter) * m_rect.width();
            qreal y = (below + belowCenter) * m_rect.height();
            bool cornerCase = (above && onLeft) || (above && onRight) || (below && onLeft) || (below && onRight);
            bool vertical = qAbs(anchor.x() - x) > qAbs(anchor.y() - y);

            qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * (onLeft * 10 - onRight * 20);
            qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * (above * 10 - below * 20) - 10;
            point1.setX(x1);
            point1.setY(y1);

            qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * (onLeft * 20 - onRight * 10);
            qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * (above * 20 - below * 10) - 10;
            point2.setX(x2);
            point2.setY(y2);

            path.moveTo(point1);
            path.lineTo(point3);
            path.lineTo(point2);
            path = path.simplified();
        }
    }
    painter->setBrush(QColor(255, 255, 255));
    painter->setPen(Qt::black);
    painter->drawPath(path);
    painter->drawText(m_textRect, m_text);

}

void Callout::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(true);
}

void Callout::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton){
        setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
        event->setAccepted(true);
    } else {
        event->setAccepted(false);
    }
}

void Callout::setText(const QString &text)
{
    m_text = text;
    QFontMetrics metrics(m_font);
    m_textRect = metrics.boundingRect(QRect(0, 0, 150, 150), Qt::AlignLeft, m_text);
    m_textRect.translate(5, -5);
    prepareGeometryChange();
    m_rect = m_textRect.adjusted(-5, -5, 5, 5);
}

void Callout::setAnchor(QPointF point)
{
    m_anchor = point;
}

void Callout::updateGeometry()
{
    prepareGeometryChange();
    setPos(m_chart->mapToPosition(m_anchor) + QPoint(10, -50));
}

void Callout::setMarkerColor(QColor color) {
    m_color = color;
    setIfDrawColor(true);
}

QColor Callout::getColor() const {
    return m_color;
}

void Callout::setIfDrawColor(bool status) {
    IfDrawMarker = status;
}

bool Callout::isIfDrawColor() const {
    return IfDrawMarker;
}

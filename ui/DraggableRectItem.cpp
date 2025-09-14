// draggablerectitem.cpp
#include "draggablerectitem.h"
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QCursor>

DraggableRectItem::DraggableRectItem(int id, QGraphicsItem *parent)
    : QGraphicsRectItem(parent), m_id(id), m_gridPosition(-1, -1)
{
    // 设置矩形属性
    setRect(0, 0, 100, 80);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    // 为每个矩形设置不同的颜色
    int hue = (id * 55) % 360; // 均匀分布颜色
    m_color = QColor::fromHsv(hue, 150, 230);
    setBrush(m_color);

    // 添加边框
    setPen(QPen(Qt::black, 2));

    // 添加文本显示ID
    m_textItem = new QGraphicsTextItem(QString::number(id), this);
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    m_textItem->setFont(font);
    m_textItem->setDefaultTextColor(Qt::white);

    // 居中文本
    QRectF textRect = m_textItem->boundingRect();
    m_textItem->setPos((rect().width() - textRect.width()) / 2,
                       (rect().height() - textRect.height()) / 2);
}

DraggableRectItem::DraggableRectItem(int id, QString description, QGraphicsItem *parent)
    : QGraphicsRectItem(parent), m_id(id), m_gridPosition(-1, -1)
{
    setRect(0, 0, 100, 80);
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    // 为每个矩形设置不同的颜色
    int hue = (id * 55) % 360; // 均匀分布颜色
    m_color = QColor::fromHsv(hue, 150, 230);
    setBrush(m_color);

    // 添加边框
    setPen(QPen(Qt::black, 2));

    // 添加文本显示ID
    m_textItem = new QGraphicsTextItem(description, this);
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    m_textItem->setFont(font);
    m_textItem->setDefaultTextColor(Qt::white);

    // 居中文本
    QRectF textRect = m_textItem->boundingRect();
    m_textItem->setPos((rect().width() - textRect.width()) / 2,
                       (rect().height() - textRect.height()) / 2);
}

void DraggableRectItem::setGridPosition(int row, int col)
{
    m_gridPosition = QPoint(row, col);
}

void DraggableRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = pos();
        setCursor(Qt::ClosedHandCursor);

        // 提高Z值使被拖动的矩形显示在最前面
        setZValue(1);
        emit itemSelected(this);
    }
    QGraphicsRectItem::mousePressEvent(event);
}

void DraggableRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // 发送拖动信号
        emit itemDragged(this, pos());
    }
    QGraphicsRectItem::mouseMoveEvent(event);
}

void DraggableRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setCursor(Qt::ArrowCursor);

        // 恢复Z值
        setZValue(0);

        // 发送释放信号
        emit itemDropped(this);
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
}



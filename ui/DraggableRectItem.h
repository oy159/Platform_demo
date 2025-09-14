// draggablerectitem.h
#ifndef DRAGGABLERECTITEM_H
#define DRAGGABLERECTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QObject>

class DraggableRectItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DraggableRectItem(int id, QString description, QGraphicsItem *parent = nullptr);
    explicit DraggableRectItem(int id, QGraphicsItem *parent = nullptr);

    int getId() const { return m_id; }
    void setGridPosition(int row, int col);
    QPoint getGridPosition() const { return m_gridPosition; }
    QRectF sceneBoundingRect() const {
        return mapToScene(boundingRect()).boundingRect();
    }
    void setOrignalPos(double x, double y) {
        m_originalPos = {x,y};
        setPos(x, y);
    };
    void setRectWidthHeight(double w, double h) {
        setRect(0, 0, w, h);
        // 重新居中文本
        QRectF textRect = m_textItem->boundingRect();
        m_textItem->setPos((rect().width() - textRect.width()) / 2,
                           (rect().height() - textRect.height()) / 2);
    }

    int getId() {
        return m_id;
    }

    QPointF m_originalPos;

signals:
    void itemSelected(DraggableRectItem* item);
    void itemDragged(DraggableRectItem* item, const QPointF& newPos);
    void itemDropped(DraggableRectItem* item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int m_id;
    QPoint m_gridPosition;
    QPointF m_dragStartPosition;
    QGraphicsTextItem* m_textItem;
    QColor m_color;

};

#endif // DRABBABLERECTITEM_H
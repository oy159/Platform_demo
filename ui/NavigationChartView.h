#ifndef NAVIGATIONCHARTVIEW_H
#define NAVIGATIONCHARTVIEW_H

#include <QChartView>
#include <QMouseEvent>
#include <QTimer>

class NavigationChartView : public QChartView {
    Q_OBJECT

public:
    using QChartView::QChartView;

    QPoint getScrollDistance() const {
        return scrollDis; // 返回当前滚动距离
    }

    void resetScrollDistance() {
        scrollDis = QPoint(0, 0); // 重置滚动距离
    }

signals:
    void zoomCompleted(); // 添加缩放完成信号

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::MiddleButton) {
            m_lastPos = event->pos();
            setCursor(Qt::ClosedHandCursor);
            return;
        }

        // 记录鼠标按下位置，用于判断是否是缩放操作
        if (event->button() == Qt::LeftButton && rubberBand() == QChartView::RectangleRubberBand) {
            m_zoomStartPos = event->pos();
            m_isZooming = true;
        }

        QChartView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::MiddleButton) {
            auto dPos = event->pos() - m_lastPos;
            chart()->scroll(-dPos.x(), dPos.y());
            scrollDis += dPos; // 更新滚动距离
            m_lastPos = event->pos();
            return;
        }
        QChartView::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::MiddleButton) {
            setCursor(Qt::ArrowCursor);
            return;
        }

        // 如果是缩放操作完成，发出信号
        if (event->button() == Qt::LeftButton && m_isZooming) {
            m_isZooming = false;

            // 检查是否真的进行了缩放（鼠标移动了一定距离）
            QPoint delta = event->pos() - m_zoomStartPos;
            if (delta.manhattanLength() > 5) { // 移动超过5像素才认为是有效缩放
                // 增加延迟时间到 500 毫秒
                QTimer::singleShot(200, this, [this]() {
                    emit zoomCompleted();
                });
            }
        }

        // 自定义右键行为
        if (event->button() == Qt::RightButton) {
            return;
        }

        QChartView::mouseReleaseEvent(event);
    }

private:
    QPoint m_lastPos;
    QPoint scrollDis = QPoint(0, 0); // 用于记录滚动距离
    QPoint m_zoomStartPos; // 缩放起始位置
    bool m_isZooming = false; // 是否正在进行缩放操作
};

#endif
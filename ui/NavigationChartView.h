#ifndef NAVIGATIONCHARTVIEW_H
#define NAVIGATIONCHARTVIEW_H


#include <QChartView>

class NavigationChartView : public QChartView {
public:
    using QChartView::QChartView;
    
    QPoint getScrollDistance() const {
        return scrollDis; // 返回当前滚动距离
    }

    void resetScrollDistance() {
        scrollDis = QPoint(0, 0); // 重置滚动距离
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::MiddleButton) {
            m_lastPos = event->pos();
            setCursor(Qt::ClosedHandCursor);
            return;
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
        
        // 自定义右键行为
        if (event->button() == Qt::RightButton) {
            // if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
            //     chart()->zoomReset(); // Shift+右键 复位视图
            // } else {
            //     return;
            //     // chart()->zoom(0.7); // 普通右键缩小更多
            // }
            return;
        }
        
        QChartView::mouseReleaseEvent(event);
    }
    
    // void wheelEvent(QWheelEvent *event) override {
    //     double factor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    //     chart()->zoom(factor);
    // }

private:
    QPoint m_lastPos;
    QPoint scrollDis = QPoint(0, 0); // 用于记录滚动距离
};


#endif
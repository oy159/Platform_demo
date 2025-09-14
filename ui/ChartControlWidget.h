// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QCheckBox>

#include "draggablerectitem.h"


struct GridPosition {
    int row;
    int col;

    GridPosition(int r = -1, int c = -1) : row(r), col(c) {}

    // 用于QDebug输出
    friend QDebug operator<<(QDebug debug, const GridPosition &pos) {
        debug << "(" << pos.row << "," << pos.col << ")";
        return debug;
    }
};

class ChartControlWidget : public QWidget
{
    Q_OBJECT

public:
    ChartControlWidget(QWidget *parent = nullptr);
    ~ChartControlWidget();

signals:
    void sendInform(QHash<int, GridPosition> &Inform);

private slots:
    void onItemSelected(DraggableRectItem* item);
    void onItemDragged(DraggableRectItem* item, const QPointF& newPos);
    void onItemDropped(DraggableRectItem* item);
    void addRectangle();
    void addRectangle(int index);
    void addChart(bool checked);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void initMenu();
    void arrangeItems();
    void swapItems(DraggableRectItem* item1, DraggableRectItem* item2);
    void deleteRectangle(int itemId);
    void deleteRectangle(DraggableRectItem* item);
    void createInformToManager();

    QGraphicsView *m_graphicsView;
    QGraphicsScene *m_scene;
    QList<DraggableRectItem*> m_items;
    DraggableRectItem* m_draggedItem;
    DraggableRectItem* m_selectedItem;
    QPointF m_dragOffset;
    QVector<QPoint> m_swapHistory;

    QMenu* menu;
    QWidgetAction* addChartRect;
    QWidget* addChartRectWidget;
    QVBoxLayout* addChartRectLayout;
    QVector<QCheckBox*> checkBoxList;

    qreal sceneWidth;
    qreal sceneHeight;
    QList<QString> ChartWidgetNames = {
        "Spectrum Chart",
        "Time Chart",
        "DNL Chart",
        "INL Chart",
        "Histogram Chart",
        "DAC Static Chart",
        "N9040B Chart",
    };

    QHash<int, GridPosition> mInform;
    bool ifPopup = true;
    // QVector<ItemPosition> *mInform;
};

#endif // MAINWINDOW_H
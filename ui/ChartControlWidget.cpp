// mainwindow.cpp
#include <qcombobox.h>

#include "ChartControlWidget.h"
#include <QDebug>
#include <qevent.h>
#include <qmenubar.h>
#include <qguiapplication.h>
#include <QMessageBox>


ChartControlWidget::ChartControlWidget(QWidget *parent)
    : QWidget(parent), m_draggedItem(nullptr), sceneWidth(600), sceneHeight(480)
{
    resize(800,600);
    setMinimumSize(600, 400);
    setWindowTitle("可拖动矩形交换位置");
    m_graphicsView = new QGraphicsView(this);
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

    m_graphicsView->setScene(m_scene);
    m_graphicsView->setRenderHint(QPainter::Antialiasing);
    m_graphicsView->setDragMode(QGraphicsView::NoDrag);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    m_graphicsView->setBackgroundBrush(QColor(240, 240, 240));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_graphicsView);

    initMenu();
    m_scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
}

ChartControlWidget::~ChartControlWidget()
{
}


void ChartControlWidget::initMenu() {
    menu = new QMenu(this);
    addChartRect = new QWidgetAction(menu);
    addChartRectWidget = new QWidget();
    addChartRectLayout = new QVBoxLayout(addChartRectWidget);
    checkBoxList.clear();
    for (const auto & chartWidgetName : ChartWidgetNames) {
        auto *checkBox = new QCheckBox(chartWidgetName, addChartRectWidget);
        addChartRectLayout->addWidget(checkBox);
        connect(checkBox, &QCheckBox::toggled, this, &ChartControlWidget::addChart);
        checkBoxList.append(checkBox);
    }
    checkBoxList[0]->setChecked(true);

    addChartRect->setDefaultWidget(addChartRectWidget);
    menu->addAction(addChartRect);
    menu->addSeparator();
    auto *allChosenAction = menu->addAction("全选");
    allChosenAction->setCheckable(true);
    allChosenAction->setChecked(false);
    connect(allChosenAction, &QAction::toggled, [this](bool status) {
        if (status) {
            for (auto checkBox : checkBoxList) {
                checkBox->setChecked(true);
            }
        }else {
            for (auto checkBox : checkBoxList) {
                checkBox->setChecked(false);
            }
        }
    });
}

void ChartControlWidget::arrangeItems()
{
    int cols;
    int rows;
    const qreal spacing = 10;
    int totalItems = m_items.size();
    rows = std::ceil(std::sqrt(totalItems));
    cols = std::ceil(static_cast<double>(totalItems) / rows);


    qreal itemWidth;
    qreal itemHeight;

    qreal startX = spacing;
    qreal startY = spacing;

    itemWidth = (sceneWidth - (cols + 1) * spacing) / cols ;
    itemHeight = (sceneHeight - (rows + 1) * spacing) / rows;

    mInform.clear();
    for (int i = 0; i < totalItems; ++i) {
        int row = i / cols;
        int col = i % cols;

        qreal x = startX + col * (itemWidth + spacing);
        qreal y = startY + row * (itemHeight + spacing);
        m_items[i]->setRectWidthHeight(itemWidth, itemHeight);
        m_items[i]->setGridPosition(row, col);
        m_items[i]->setOrignalPos(x, y);

        mInform[m_items[i]->getId()] = GridPosition(row, col);

    }
}

void ChartControlWidget::onItemSelected(DraggableRectItem *item) {
    m_selectedItem = item;
}

void ChartControlWidget::onItemDragged(DraggableRectItem* item, const QPointF& newPos)
{
    if (!m_draggedItem) {
        m_draggedItem = item;
        m_dragOffset = item->pos() - newPos;
    }
}

void ChartControlWidget::onItemDropped(DraggableRectItem* item)
{
    if (m_draggedItem) {
        DraggableRectItem* targetItem = nullptr;
        qreal maxOverlapRatio = 0;
        for (DraggableRectItem* otherItem : m_items) {
            if (otherItem == item) continue;
            QRectF itemRect = item->sceneBoundingRect();
            QRectF otherRect = otherItem->sceneBoundingRect();
            QRectF overlapRect = itemRect.intersected(otherRect);
            if (overlapRect.isValid()) {
                qreal itemArea = itemRect.width() * itemRect.height();
                qreal overlapArea = overlapRect.width() * overlapRect.height();
                qreal overlapRatio = (itemArea > 0) ? overlapArea / itemArea : 0;
                if (overlapRatio > maxOverlapRatio) {
                    maxOverlapRatio = overlapRatio;
                    targetItem = otherItem;
                }
            }
        }
        if (targetItem && maxOverlapRatio > 0.5) {
            qDebug() << "Overlap ratio:" << maxOverlapRatio << ", swapping items";
            swapItems(item, targetItem);
        } else {
            qDebug() << "Returning to original position";
            item->setPos(item->m_originalPos);  // 恢复原始位置
        }
        m_draggedItem = nullptr;
    }
}

void ChartControlWidget::swapItems(DraggableRectItem* item1, DraggableRectItem* item2)
{
    auto pos1 = item1->m_originalPos;
    auto pos2 = item2->m_originalPos;
    item1->setPos(pos2);
    item2->setPos(pos1);
    item1->m_originalPos = pos2;
    item2->m_originalPos = pos1;

    auto grid1 = item1->getGridPosition();
    auto grid2 = item2->getGridPosition();
    qDebug() << "Position1:" << mInform[item1->getId()];
    qDebug() << "Position2:" << mInform[item2->getId()];
    mInform[item1->getId()] = GridPosition(grid2.x(), grid2.y());
    mInform[item2->getId()] = GridPosition(grid1.x(), grid1.y());
    qDebug() << "Position1:" << mInform[item1->getId()];
    qDebug() << "Position2:" << mInform[item2->getId()];
    item1->setGridPosition(grid2.x(), grid2.y());
    item2->setGridPosition(grid1.x(), grid1.y());


    qDebug() << "Swapped item" << item1->getId() << "with item" << item2->getId();

    // 可选：记录交换历史
    m_swapHistory.push_back({item1->getId(), item2->getId()});
}


void ChartControlWidget::addRectangle()
{
    // 创建新的矩形，ID从当前矩形数量开始
    int newId = m_items.size();
    DraggableRectItem *newItem = new DraggableRectItem(newId);
    connect(newItem, &DraggableRectItem::itemSelected, this, &ChartControlWidget::onItemSelected);
    connect(newItem, &DraggableRectItem::itemDragged, this, &ChartControlWidget::onItemDragged);
    connect(newItem, &DraggableRectItem::itemDropped, this, &ChartControlWidget::onItemDropped);

    m_items.append(newItem);
    m_scene->addItem(newItem);

    arrangeItems();
}

void ChartControlWidget::addRectangle(int index) {
    DraggableRectItem *newItem = new DraggableRectItem(index, ChartWidgetNames[index]);
    connect(newItem, &DraggableRectItem::itemSelected, this, &ChartControlWidget::onItemSelected);
    connect(newItem, &DraggableRectItem::itemDragged, this, &ChartControlWidget::onItemDragged);
    connect(newItem, &DraggableRectItem::itemDropped, this, &ChartControlWidget::onItemDropped);

    m_items.append(newItem);
    m_scene->addItem(newItem);

    arrangeItems();
}

void ChartControlWidget::deleteRectangle(int itemId) {
    // 遍历所有项目，找到具有指定ID的项目
    for (int i = 0; i < m_items.size(); ++i) {
        DraggableRectItem *item = m_items.at(i);
        if (item->getId() == itemId) { // 假设getIndex()返回项目的ID
            // 从场景中移除项目
            m_scene->removeItem(item);
            m_items.removeAt(i);
            delete item;
            arrangeItems();
            return; // 找到并删除后立即返回
        }
    }
    qWarning() << "Item with ID" << itemId << "not found for deletion";
}

void ChartControlWidget::deleteRectangle(DraggableRectItem *item) {
    for (int i = 0; i < m_items.size(); ++i) {
        DraggableRectItem *item1 = m_items.at(i);
        if (item1 == item) { // 假设getIndex()返回项目的ID
            // 从场景中移除项目
            m_scene->removeItem(item1);
            m_items.removeAt(i);
            delete item1;
            arrangeItems();
            return; // 找到并删除后立即返回
        }
    }
}

void ChartControlWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    sceneWidth = (m_graphicsView->size()).width() - 10;
    sceneHeight = (m_graphicsView->size()).height() - 10;
    m_scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
    arrangeItems();

}

void ChartControlWidget::contextMenuEvent(QContextMenuEvent *event){
    menu->exec(event->globalPos());
}

void ChartControlWidget::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent(event);
    QMessageBox::StandardButton reply;
    if (ifPopup) {
        reply = QMessageBox::question(this, "是否保存图窗设置", "是否保存图窗设置？",
                                     QMessageBox::Yes | QMessageBox::No);
    }else {
        reply = QMessageBox::Yes;
    }

    if (reply == QMessageBox::Yes) {
        emit sendInform(mInform);
        ifPopup = true;
        event->accept();
    } else {
        event->ignore();
    }
}

void ChartControlWidget::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
    if (event->key() == Qt::Key_Delete && m_selectedItem != nullptr) {
        checkBoxList[m_selectedItem->getId()]->setChecked(false);
    }else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_S) {
        ifPopup = false;
        close();
    }


}

void ChartControlWidget::addChart(bool checked) {
    auto *checkBox = qobject_cast<QCheckBox *>(sender());
    int index = checkBoxList.indexOf(checkBox);
    if (checked) {
        addRectangle(index);
    }else {
        deleteRectangle(index);
    }
}


void ChartControlWidget::createInformToManager() {

}
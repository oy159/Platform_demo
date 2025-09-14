#include "ChartWidgetsManager.h"
#include <QDebug>
#include <QDataStream>

void ChartWidgetsManager::reorganizeCharts() {
    // 清除现有布局
    QLayoutItem *child;
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->setParent(nullptr);
        delete child;
    }

    if (chartWidgets.isEmpty()) return;

    QList<int> toShow = visibleIndexes;
    int count = toShow.size();
    if (count == 0) return;

    int rows = qCeil(qSqrt(count));
    int cols = qCeil(count / double(rows));
    int idx = 0;
    for (int i : toShow) {
        int row = idx / cols;
        int col = idx % cols;
        gridLayout->addWidget(chartWidgets[i], row, col);
        ++idx;
    }
    for (int r = 1; r < gridLayout->rowCount(); ++r)
        gridLayout->setRowStretch(r, 0);
    for (int c = 1; c < gridLayout->columnCount(); ++c)
        gridLayout->setColumnStretch(c, 0);
    for (int r = 0; r < rows; ++r)
        gridLayout->setRowStretch(r, 1);
    for (int c = 0; c < cols; ++c)
        gridLayout->setColumnStretch(c, 1);

    // 更新当前布局信息
    currentLayoutInform.clear();
    idx = 0;
    for (int i : toShow) {
        int row = idx / cols;
        int col = idx % cols;
        currentLayoutInform.insert(i, GridPosition(row, col));
        ++idx;
    }
}

void ChartWidgetsManager::reorganizeFromInform(const QHash<int, GridPosition> &inform) {
    // 清除现有布局
    QLayoutItem *child;
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->setParent(nullptr);
        delete child;
    }

    // 重置所有行列的拉伸设置
    for (int r = 0; r < gridLayout->rowCount(); ++r)
        gridLayout->setRowStretch(r, 0);
    for (int c = 0; c < gridLayout->columnCount(); ++c)
        gridLayout->setColumnStretch(c, 0);

    // 找出最大的行和列，以便设置拉伸
    int maxRow = 0;
    int maxCol = 0;

    // 根据接收到的布局信息放置图表
    for (auto it = inform.begin(); it != inform.end(); ++it) {
        int chartIndex = it.key();
        GridPosition pos = it.value();

        // 确保索引有效
        if (chartIndex >= 0 && chartIndex < chartWidgets.size()) {
            gridLayout->addWidget(chartWidgets[chartIndex], pos.row, pos.col);

            // 更新最大行和列
            if (pos.row > maxRow) maxRow = pos.row;
            if (pos.col > maxCol) maxCol = pos.col;
        }
    }

    // 设置行和列的拉伸
    for (int r = 0; r <= maxRow; ++r)
        gridLayout->setRowStretch(r, 1);
    for (int c = 0; c <= maxCol; ++c)
        gridLayout->setColumnStretch(c, 1);

    // 更新当前布局信息和可见索引
    currentLayoutInform = inform;
    visibleIndexes = inform.keys();

}

void ChartWidgetsManager::receiveLayoutInform(QHash<int, GridPosition> &inform) {
    reorganizeFromInform(inform);
}

void ChartWidgetsManager::showContextMenu(const QPoint &pos) {
    QMenu menu(this);
    menu.addSeparator();
    QList<QAction*> actions;

    if(chartWidgets.size() > ChartWidgetNames.size()) {
        qDebug() << "ChartWidgetsManager: chartWidgets size exceeds ChartWidgetNames size!";
    }

    for (int i = 0; i < ChartWidgetNames.size(); ++i) {
        QString title = ChartWidgetNames[i];
        QAction *action = menu.addAction(title);
        action->setCheckable(true);
        action->setChecked(visibleIndexes.contains(i));
        action->setData(i);
        actions.append(action);
    }
    QAction *selected = menu.exec(mapToGlobal(pos));
    if (selected) {

        int idx = selected->data().toInt();
        showAll = false;
        if (visibleIndexes.contains(idx)) {
            visibleIndexes.removeAll(idx);
        } else {
            visibleIndexes.append(idx);
        }
        reorganizeCharts();

    }
}
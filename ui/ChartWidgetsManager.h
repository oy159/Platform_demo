#ifndef CHARTWIDGETSMANAGER_H
#define CHARTWIDGETSMANAGER_H


#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

class ChartWidgetsManager : public QWidget {
    Q_OBJECT
public:
    explicit ChartWidgetsManager(QWidget *parent = nullptr)
        : QWidget(parent), gridLayout(new QGridLayout(this)), showAll(false), visibleIndexes{0} {
        setLayout(gridLayout);
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QWidget::customContextMenuRequested,
                this, &ChartWidgetsManager::showContextMenu);
    }

    void addChart(QWidget* chart) {
        chartWidgets.append(chart);
    }

    void reorganizeCharts() {
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
            // chartWidgets[i]->setVisible(true);
            int row = idx / cols;
            int col = idx % cols;
            gridLayout->addWidget(chartWidgets[i], row, col);
            ++idx;
        }
        // 隐藏未显示的窗口
        // for (int i = 0; i < chartWidgets.size(); ++i) {
        //     if (!toShow.contains(i)) chartWidgets[i]->setVisible(false);
        // }
        // 拉伸因子
        for (int r = 1; r < gridLayout->rowCount(); ++r)
            gridLayout->setRowStretch(r, 0);
        for (int c = 1; c < gridLayout->columnCount(); ++c)
            gridLayout->setColumnStretch(c, 0);
        for (int r = 0; r < rows; ++r)
            gridLayout->setRowStretch(r, 1);
        for (int c = 0; c < cols; ++c)
            gridLayout->setColumnStretch(c, 1);
    }

    int chartCount() const { return chartWidgets.size(); }

private slots:
    void showContextMenu(const QPoint &pos) {
        QMenu menu(this);
        // QAction *showAllAction = menu.addAction("显示全部窗口");
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
            // 切换该窗口的显示状态
            if (visibleIndexes.contains(idx)) {
                visibleIndexes.removeAll(idx);
            } else {
                visibleIndexes.append(idx);
            }
            reorganizeCharts();
            
        }
    }

private:
    QGridLayout *gridLayout;
    QList<QWidget*> chartWidgets;
    bool showAll = false;
    QList<int> visibleIndexes;

    QList<int> getAllIndexes() const {
        QList<int> idxs;
        for (int i = 0; i < chartWidgets.size(); ++i) idxs.append(i);
        return idxs;
    }

    QList<QString> ChartWidgetNames = {
        "Spectrum Chart",
        "Time Chart",
        "DNL Chart",
        "INL Chart",
        "Histogram Chart"
    };
};













#endif// CHARTWIDGETSMANAGER_H
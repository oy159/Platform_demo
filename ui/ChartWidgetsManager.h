#ifndef CHARTWIDGETSMANAGER_H
#define CHARTWIDGETSMANAGER_H


#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <QMenu>
#include <QAction>
#include <ChartControlWidget.h>
#include <SpectrumChartTryWidget.h>

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

    QHash<int, GridPosition> getCurrentLayout() const {
        return currentLayoutInform;
    }

    void reorganizeCharts();
    void reorganizeFromInform(const QHash<int, GridPosition> &inform);

    int chartCount() const { return chartWidgets.size(); }
public slots:
    void receiveLayoutInform(QHash<int, GridPosition> &inform);

private slots:
    void showContextMenu(const QPoint &pos);

private:
    QGridLayout *gridLayout;
    QList<QWidget*> chartWidgets;
    bool showAll = false;
    QList<int> visibleIndexes;
    QHash<int, GridPosition> currentLayoutInform;

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
        "Histogram Chart",
        "DAC Static Chart",
        "N9040B Chart",
    };
};



#endif// CHARTWIDGETSMANAGER_H
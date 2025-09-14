#include "BaseChartWidget.h"
#include "callout.h"
#include <QVBoxLayout>
#include <QDebug>

BaseChartWidget::BaseChartWidget(QWidget *parent) 
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    initChart();
    initMenu();
    mainLayout->addWidget(m_chartView);

    m_cursor = new Cursor(m_chart);

// 设置初始位置
    m_cursor->setVerticalLine1Pos(300);
    m_cursor->setVerticalLine2Pos(600);
    m_cursor->setHorizontalLine1Pos(200);
    m_cursor->setHorizontalLine2Pos(700);

// 自定义样式
    m_cursor->setLineColor(Qt::green);
    m_cursor->setTextColor(Qt::black);
    m_cursor->setLineStyle(Qt::DotLine);
    m_cursor->setFont(QFont("Arial", 8));

// 控制可见性
    m_cursor->setVerticalLinesVisible(true);
    m_cursor->setHorizontalLinesVisible(true);
    m_cursor->setTextVisible(true);

    m_cursor->hide();
    loadStyleSheet();

}

BaseChartWidget::~BaseChartWidget() {
    delete m_chart;
}

void BaseChartWidget::initChart() {
    m_chart = new QChart();
    m_chart->setAnimationOptions(QChart::NoAnimation);
    m_chart->setMargins(QMargins(5, 5, 20, 5));

    m_series = new QLineSeries();
    m_series->setUseOpenGL(true);
    
    QPen pen(Qt::blue);
    pen.setWidthF(1.5);
    m_series->setPen(pen);
    
    m_chart->addSeries(m_series);
    
    // 初始化坐标轴
    m_axisX = new QValueAxis;
    m_axisY = new QValueAxis;
    
    m_axisX->setTitleText("X");
    m_axisX->setLabelFormat(QString("%.%1f").arg(m_xPrecision));
    m_axisX->setTickCount(11);
    
    m_axisY->setTitleText("Y");
    m_axisY->setLabelFormat(QString("%.%1f").arg(m_yPrecision));
    m_axisY->setTickCount(11);
    
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
    
    // 标记点序列
    m_scatterSeries = new QScatterSeries();
    m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_scatterSeries->setMarkerSize(10.0);
    m_scatterSeries->setColor(Qt::transparent); // 散点不可见但信号可用
    m_scatterSeries->setBorderColor(Qt::transparent);
    m_chart->addSeries(m_scatterSeries);
    m_scatterSeries->attachAxis(m_axisX);
    m_scatterSeries->attachAxis(m_axisY);
    
    m_chartView = new NavigationChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing, true);
    m_chartView->setRenderHint(QPainter::SmoothPixmapTransform, true);
    m_chartView->setDragMode(QGraphicsView::RubberBandDrag);
    m_chartView->setMouseTracking(true);
    // m_chartView->setRubberBand(QChartView::RectangleRubberBand);
    m_chart->legend()->hide();
    
    connect(m_series, &QLineSeries::clicked, this, &BaseChartWidget::keepCallout);
    // connect(m_series, &QLineSeries::hovered, this, &BaseChartWidget::tooltip);

    m_scatterSeries->setVisible(true);
    connect(m_scatterSeries, &QScatterSeries::clicked, this, &BaseChartWidget::keepCallout);
    connect(m_scatterSeries, &QScatterSeries::hovered, this, &BaseChartWidget::tooltip);
    
    // 坐标显示
    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    // m_coordX->setText("X: ");
    m_coordY = new QGraphicsSimpleTextItem(m_chart);
    // m_coordY->setText("Y: ");
    m_coordX->setBrush(Qt::black);
    m_coordY->setBrush(Qt::black);
}

void BaseChartWidget::initMenu() {
    menu = new QMenu(this);
    snipAction = menu->addAction("截图");
    connect(snipAction, &QAction::triggered, this, &BaseChartWidget::captrueScreen);

    exportDataAction = menu->addAction("数据导出");
    connect(exportDataAction, &QAction::triggered, this, &BaseChartWidget::exportData);

    settingSubMenu = menu->addMenu("功能");
    ControlCursorAction = settingSubMenu->addAction("显示迹线");
    ControlCursorAction->setCheckable(true);
    ControlCursorAction->setChecked(_ifShowCursor);
    connect(ControlCursorAction, &QAction::toggled, this, &BaseChartWidget::setCursorVisible);


    ZoomOutAction = settingSubMenu->addAction("放大");

    connect(ZoomOutAction, &QAction::triggered, [this]() {
        // 启用矩形缩放模式
        m_chartView->setRubberBand(QChartView::RectangleRubberBand);
        m_chartView->setCursor(Qt::CrossCursor);
    });

    // 连接缩放完成信号
    connect(m_chartView, &NavigationChartView::zoomCompleted, this, [this]() {
        // 缩放完成后关闭缩放模式
        m_chartView->setRubberBand(QChartView::NoRubberBand);
        m_chartView->setCursor(Qt::ArrowCursor);
    });

    markerSubMenu = settingSubMenu->addMenu("Marker");

    markerActions.clear();
    marker1Action = new MarkerColorAction("Marker1", Qt::red, markerSubMenu);
    marker1Action->setCheckable(true);
    marker1Action->setChecked(true);
    markerSubMenu->addAction(marker1Action);
    markerActions.append(marker1Action);
    connect(marker1Action, &MarkerColorAction::checkedChanged, this, &BaseChartWidget::setMarkerToggled);
    m_tooltip = new Callout(m_chart);
    m_tooltip->setMarkerColor(marker1Action->color());

    markerSubMenu->addSeparator();
    addMarkerAction = markerSubMenu->addAction("添加标记点");
    connect(addMarkerAction, &QAction::triggered, this, &BaseChartWidget::addMarker);
    ClearMarkerAction = markerSubMenu->addAction("清除所有标记点");
    connect(ClearMarkerAction, &QAction::triggered, [this]() {
        QList<Callout*> calloutsToRemove = m_callouts;
        for (auto* callout : m_callouts) {
            if (callout) {
                callout->hide();
                callout->setIfDrawColor(false);
                m_calloutPool.append(callout);
            }
        }
        m_callouts.clear();
        mPeaksVisible.resize(mPeaks.size(), false);
        marker1Action->setChecked(true);
    });

    findMaxAction = markerSubMenu->addAction("查找最大值");
    connect(findMaxAction, &QAction::triggered, this, &BaseChartWidget::findMaxPoint);
    findNextAction = markerSubMenu->addAction("查找下一个");
    connect(findNextAction, &QAction::triggered, this, &BaseChartWidget::findNextPoint);


    axisYSubMenu = menu->addMenu("Y轴");
    axisYPrecision = new QWidgetAction(axisYSubMenu);
    axisYPrecisionWidget = new QWidget();
    axisYPrecisionLayout = new QHBoxLayout(axisYPrecisionWidget);
    axisYPrecisionLabel = new QLabel("波形图Y轴小数位数");
    axisYPrecisionSB = new QSpinBox();
    axisYPrecisionSB->setRange(0, 3);
    connect(axisYPrecisionSB, &QSpinBox::valueChanged, axisYPrecisionWidget, [this](int value) {
        m_yPrecision = value;
        // 更新Y轴标签格式
        double maxY = qMax(qAbs(m_axisY->max()), qAbs(m_axisY->min()));
        if (maxY >= 1000.0) {
            m_axisY->setLabelFormat(QString("%.%1e").arg(m_yPrecision));
        } else {
            m_axisY->setLabelFormat(QString("%.%1f").arg(m_yPrecision));
        }
    });

    axisYPrecisionLayout->addWidget(axisYPrecisionLabel);
    axisYPrecisionLayout->addWidget(axisYPrecisionSB);

    axisYPrecisionWidget->setLayout(axisYPrecisionLayout);
    axisYPrecision->setDefaultWidget(axisYPrecisionWidget);
    axisYSubMenu->addAction(axisYPrecision);


    axisXSubMenu = menu->addMenu("X轴");
    axisXPrecision = new QWidgetAction(axisXSubMenu);
    axisXPrecisionWidget = new QWidget();
    axisXPrecisionLayout = new QHBoxLayout(axisXPrecisionWidget);
    axisXPrecisionLabel = new QLabel("波形图X轴小数位数");
    axisXPrecisionSB = new QSpinBox();
    axisXPrecisionSB->setRange(0, 3);
    connect(axisXPrecisionSB, &QSpinBox::valueChanged, axisXPrecisionWidget, [this](int value) {
        m_xPrecision = value;
        // 更新X轴标签格式
        double maxX = qMax(qAbs(m_axisX->max()), qAbs(m_axisX->min()));
        if (maxX >= 1000.0) {
            m_axisX->setLabelFormat(QString("%.%1e").arg(m_xPrecision));
        } else {
            m_axisX->setLabelFormat(QString("%.%1f").arg(m_xPrecision));
        }
    });

    axisXPrecisionLayout->addWidget(axisXPrecisionLabel);
    axisXPrecisionLayout->addWidget(axisXPrecisionSB);

    axisXPrecisionWidget->setLayout(axisXPrecisionLayout);
    axisXPrecision->setDefaultWidget(axisXPrecisionWidget);
    axisXSubMenu->addAction(axisXPrecision);



    menu->addSeparator();
    autoAdjustAction = menu->addAction("AUTO");
    connect(autoAdjustAction, &QAction::triggered, this, &BaseChartWidget::autoAdjust);
}

// 主线程计算峰值点
void BaseChartWidget::updateChartDataDirect(std::vector<double> data) {
    QVector<QPointF> chartData;

    int DataNum = data.size();
    
    for (size_t i = 0; i < data.size(); ++i) {
        chartData.append(QPointF(i, data[i]));
    }

    updateChartData(chartData);
    mPeaks.clear();
    mPeaks = findPeaks(data);
    std::sort(mPeaks.begin(), mPeaks.end(),
             [](const QPointF& a, const QPointF& b) { return a.y() > b.y(); });
    mPeaksVisible.resize(mPeaks.size(), false);

}

// 额外传Peak即可
void BaseChartWidget::updateChartData(const QVector<QPointF> &data) {
    if (data.isEmpty()) return;

    m_series->clear();
    m_scatterSeries->clear();

    m_series->replace(data);
    m_scatterSeries->replace(data);
    // only first
    if(first){
        optimizeAxisRanges(data);
        first = false;
    }
}



void BaseChartWidget::optimizeAxisRanges(const QVector<QPointF> &data) {
    if (data.isEmpty()) return;

    // 计算数据的实际范围
    double minX = data.first().x();
    double maxX = data.last().x();
    double minY = data.first().y();
    double maxY = data.first().y();

    for (const auto &point : data) {
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }

    // 记录原始范围用于恢复
    optimizeAxisRange(minX, maxX, 0);
    m_axisX->setRange(minX, maxX);
    m_recoverXMin = minX;
    m_recoverXMax = maxX;

    optimizeAxisRange(minY, maxY);
    m_axisY->setRange(minY, maxY);
    m_recoverYMin = minY;
    m_recoverYMax = minY;
}

void BaseChartWidget::optimizeAxisRange(double &min, double &max, double marginRatio) {
    // 计算数据范围
    double range = max - min;
    range *= 1 + marginRatio;

    // 计算合适的刻度间隔
    double tickInterval = calculateNiceTickInterval(range);

    double margin = (max - min) * marginRatio/2;
    min -= margin;
    max += margin;

    // 调整最小值和最大值到最近的刻度倍数
    min = floor(min / tickInterval) * tickInterval;
    max = ceil(max / tickInterval) * tickInterval;
}

double BaseChartWidget::calculateNiceTickInterval(double range) {
    // 计算数量级
    double magnitude = pow(10, floor(log10(range)));
    // 归一化范围
    double normalizedRange = range / magnitude;

    // 选择合适的刻度间隔
    double tickInterval;
    if (normalizedRange < 2.0) {
        tickInterval = 0.2 * magnitude;
    } else if (normalizedRange < 5.0) {
        tickInterval = 0.5 * magnitude;
    } else if (normalizedRange < 10.0) {
        tickInterval = 1.0 * magnitude;
    } else {
        tickInterval = 2.0 * magnitude;
    }
    return tickInterval;
}

void BaseChartWidget::setRecoverRange(double xMin, double xMax, double yMin, double yMax) {
    m_recoverXMin = xMin;
    m_recoverXMax = xMax;
    m_recoverYMin = yMin;
    m_recoverYMax = yMax;
}

void BaseChartWidget::resizeEvent(QResizeEvent *event)
{

    m_chart->resize(event->size());

    const auto callouts = m_callouts;
    for (Callout *callout : callouts)
        callout->updateGeometry();
    

    resize(size());
}

void BaseChartWidget::keepCallout()
{
    m_callouts.append(m_tooltip);
    if (!m_calloutPool.isEmpty()) {
        m_tooltip = m_calloutPool.takeFirst();
    } else {
        m_tooltip = new Callout(m_chart);
    }
    // if (!MarkerCalloutFlag[MarkerIndex].first) {
    //     MarkerCalloutFlag[MarkerIndex] = {true, m_callouts.size() - 1};
    // }
}

void BaseChartWidget::tooltip(QPointF point, bool state)
{
    if (m_tooltip == nullptr) {
        if (!m_calloutPool.isEmpty()) {
            m_tooltip = m_calloutPool.takeFirst();
        } else {
            m_tooltip = new Callout(m_chart);
        }
    }

    if (state) {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));

        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
        updateCoordText(point);
        // if (!MarkerCalloutFlag[MarkerIndex].first) {
        //     auto* marker = m_MarkerSeries[MarkerIndex];
        //     marker->setColor(markerActions[MarkerIndex]->color());
        //     marker->clear();
        //     marker->append(point);
        // }
    } else {
        m_tooltip->hide();
    }
}

void BaseChartWidget::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // Ctrl + 滚轮，调整 X 轴范围
        QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).first());
        if (event->angleDelta().y() > 0) {
            // 放大 X 轴（缩小范围）
            xAxis->setRange(xAxis->min(), xAxis->max() * 0.8);
        } else {
            // 缩小 X 轴（放大范围）
            xAxis->setRange(xAxis->min(), xAxis->max() * 1.25);
        }
    }else{
        // 普通滚轮，调整 Y 轴范围
        QValueAxis *yAxis = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).first());
        if (yAxis) {
            double rangeMin = yAxis->min();
            double rangeMax = yAxis->max();

            if (event->angleDelta().y() > 0) {
                // 放大 Y 轴（缩小范围）
                yAxis->setRange(rangeMin, rangeMax * 0.8);
            } else {
                // 缩小 Y 轴（放大范围）
                yAxis->setRange(rangeMin, rangeMax * 1.25);
            }
        }

    }
    event->accept();
}

// todo：需要大幅度修改
void BaseChartWidget::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
}


void BaseChartWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        optimizeAxisRanges(m_series->points());
    }
    QWidget::mouseDoubleClickEvent(event);
}



void BaseChartWidget::contextMenuEvent(QContextMenuEvent *event){
    menu->exec(event->globalPos());
}

void BaseChartWidget::addMarker() {
    int markerCount = markerActions.size() + 1;
    QString markerName = QString("Marker%1").arg(markerCount);

    // 可以选择不同的颜色，这里使用循环的颜色列表
    static QList<QColor> colors = {Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::cyan, Qt::magenta, Qt::darkRed};
    QColor color = colors[(markerCount - 1) % colors.size()];

    MarkerColorAction* newMarkerAction = new MarkerColorAction(markerName, color, markerSubMenu);
    newMarkerAction->setCheckable(true);
    newMarkerAction->setChecked(false);

    // 添加到管理向量
    markerActions.append(newMarkerAction);

    // 将新标记点插入到分隔符之前
    QList<QAction*> actions = markerSubMenu->actions();
    QAction* beforeAction = nullptr;
    for (QAction* action : actions) {
        if (action->isSeparator()) {
            beforeAction = action;
            break;
        }
    }

    if (beforeAction) {
        markerSubMenu->insertAction(beforeAction, newMarkerAction);
    } else {
        markerSubMenu->addAction(newMarkerAction);
    }

    connect(newMarkerAction, &MarkerColorAction::checkedChanged, this, &BaseChartWidget::setMarkerToggled);
}

void BaseChartWidget::setMarkerToggled(bool checked) {
    if (auto* action = qobject_cast<MarkerColorAction*>(sender())) {

        if (checked) {
            m_tooltip->setMarkerColor(action->color());
            for (MarkerColorAction* otherAction : markerActions) {
                if (otherAction != action) {
                    otherAction->setChecked(false);
                }
            }
        } else {
            bool anyChecked = false;
            for (MarkerColorAction* otherAction : markerActions) {
                if (otherAction->isChecked()) {
                    anyChecked = true;
                    break;
                }
            }
            if (!anyChecked) {
                action->setChecked(true);
            }
        }

    }
}

void BaseChartWidget::setCursorVisible(bool visible) {
    if (visible) {
        optimizeCursor();

        m_cursor->setVerticalLinesVisible(true);
        m_cursor->setHorizontalLinesVisible(true);
        m_cursor->setTextVisible(true);
        m_cursor->show();
        _ifShowCursor = true;
    }else {
        m_cursor->hide();
        _ifShowCursor = false;
    }
}

void BaseChartWidget::exportData() {
    QString fileName = QFileDialog::getSaveFileName(this, "保存CSV文件", "", "CSV文件 (*.csv)");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "错误", "无法创建文件: " + file.errorString());
        return;
    }

    QTextStream out(&file);

    // 写入CSV表头
    out << "X坐标,Y坐标\n";


    // 写入数据
    for (const QPointF &point : m_series->points()) {
        out << point.x() << "," << point.y() << "\n";
    }

    file.close();


    qDebug() << "数据已成功导出到: " << fileName;
}

void BaseChartWidget::captrueScreen() {

    // 检查screenshots文件夹是否存在，否则创建
    if (!QDir("./screenshots").exists()) {
        QDir().mkdir("./screenshots");
    }

    QPixmap pixmap = m_chartView->grab();
    QString fileName = QFileDialog::getSaveFileName(this, "保存截图", "./screenshots/img", "PNG Files (*.png);;JPEG Files (*.jpg);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        pixmap.save(fileName);
    }
}



void BaseChartWidget::loadStyleSheet() {
    QFile styleFile(":/qss/QSpinBox.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream styleStream(&styleFile);
        QString styleSheet = this->styleSheet();
        styleSheet += styleStream.readAll();
        this->setStyleSheet(styleSheet);
        styleFile.close();
    }else{
        qDebug() << "111";
    }

}

void BaseChartWidget::optimizeCursor() const {
    double xMin = m_axisX->min();
    double xMax = m_axisX->max();
    double yMin = m_axisY->min();
    double yMax = m_axisY->max();

    double x1 = xMin + (xMax - xMin) / 3.0;
    double x2 = xMin + (xMax - xMin) * 2.0 / 3.0;
    double y1 = yMin + (yMax - yMin) / 3.0;
    double y2 = yMin + (yMax - yMin) * 2.0 / 3.0;

    m_cursor->setVerticalLine1Pos(x1);
    m_cursor->setVerticalLine2Pos(x2);
    m_cursor->setHorizontalLine1Pos(y1);
    m_cursor->setHorizontalLine2Pos(y2);
}

void BaseChartWidget::findMaxPoint() {
    if (mPeaks.isEmpty()) return;

    if (!mPeaksVisible[0]) {
        if (m_tooltip == nullptr) {
            if (!m_calloutPool.isEmpty()) {
                m_tooltip = m_calloutPool.takeFirst();
            } else {
                m_tooltip = new Callout(m_chart);
            }
        }

        m_tooltip->setText(QString("最大值\nX: %1\nY: %2").arg(mPeaks[0].x()).arg(mPeaks[0].y()));
        m_tooltip->setAnchor(mPeaks[0]);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
        m_callouts.append(m_tooltip);
        if (!m_calloutPool.isEmpty()) {
            m_tooltip = m_calloutPool.takeFirst();
        } else {
            m_tooltip = new Callout(m_chart);
        }
        updateCoordText(mPeaks[0]);
        // 记录当前索引
        mCurrentPeakIndex = 0;
        mPeaksVisible[0] = true;
    }
}

void BaseChartWidget::findNextPoint() {
    if (mPeaks.isEmpty()) return;
    if (mCurrentPeakIndex < 0 || mCurrentPeakIndex >= mPeaks.size() - 1)
        mCurrentPeakIndex = 0;
    else
        ++mCurrentPeakIndex;

    QPointF nextPeak = mPeaks[mCurrentPeakIndex];

    if (!mPeaksVisible[mCurrentPeakIndex]) {
        if (m_tooltip == nullptr) {
            if (!m_calloutPool.isEmpty()) {
                m_tooltip = m_calloutPool.takeFirst();
            } else {
                m_tooltip = new Callout(m_chart);
            }
        }

        m_tooltip->setText(QString("峰值\nX: %1\nY: %2").arg(nextPeak.x()).arg(nextPeak.y()));
        m_tooltip->setAnchor(nextPeak);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
        m_callouts.append(m_tooltip);
        if (!m_calloutPool.isEmpty()) {
            m_tooltip = m_calloutPool.takeFirst();
        } else {
            m_tooltip = new Callout(m_chart);
        }
        mPeaksVisible[mCurrentPeakIndex] = true;
        updateCoordText(nextPeak);
    }
}

void BaseChartWidget::findPoint(int index) {
    if (mPeaks.isEmpty()) return;
    if (index < 0 || index >= mPeaks.size())
        return;
    mCurrentPeakIndex = index;
    QPointF nextPeak = mPeaks[mCurrentPeakIndex];

    if (!mPeaksVisible[mCurrentPeakIndex]) {
        if (m_tooltip == nullptr) {
            if (!m_calloutPool.isEmpty()) {
                m_tooltip = m_calloutPool.takeFirst();
            } else {
                m_tooltip = new Callout(m_chart);
            }
        }

        m_tooltip->setText(QString("峰值\nX: %1\nY: %2").arg(nextPeak.x()).arg(nextPeak.y()));
        m_tooltip->setAnchor(nextPeak);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
        m_callouts.append(m_tooltip);
        mPeaksVisible[mCurrentPeakIndex] = true;
        updateCoordText(nextPeak);
    }
}

void BaseChartWidget::findPoint(QPointF point) {
    if (m_tooltip == nullptr) {
        if (!m_calloutPool.isEmpty()) {
            m_tooltip = m_calloutPool.takeFirst();
        } else {
            m_tooltip = new Callout(m_chart);
        }
    }

    m_tooltip->setText(QString("峰值\nX: %1\nY: %2").arg(point.x()).arg(point.y()));
    m_tooltip->setAnchor(point);
    m_tooltip->setZValue(11);
    m_tooltip->updateGeometry();
    m_tooltip->show();
    m_callouts.append(m_tooltip);
    updateCoordText(point);

}

QVector<QPointF> BaseChartWidget::findPeaks(const std::vector<double>& data,
                                            double minHeight,
                                            size_t minDistance){
    QVector<QPointF> peaks;

    if (data.size() < 1) return peaks;

    // 第一步：找到所有候选峰值
    for (int i = 0; i < data.size() - 1; ++i) {
        if (data[i] > data[i-1] && data[i] > data[i+1] && data[i] > minHeight) {
            peaks.push_back({static_cast<double>(i), data[i]});
        }
    }

    // 第二步：根据最小距离筛选峰值
    if (minDistance > 1 && !peaks.empty()) {
        QVector<QPointF> filteredPeaks;
        filteredPeaks.push_back(peaks[0]);

        for (int i = 1; i < peaks.size(); ++i) {
            if (peaks[i].x() - filteredPeaks.back().x() >= static_cast<double>(minDistance)) {
                filteredPeaks.push_back(peaks[i]);
            } else if (peaks[i].y() > filteredPeaks.back().y()) {
                filteredPeaks.back() = peaks[i]; // 保留更高的峰值
            }
        }

        return filteredPeaks;
    }
    return peaks;
}

void BaseChartWidget::updateCoordText(const QPointF& chartPos) {
    m_coordX->setText(QString("X: %1").arg(chartPos.x()));
    m_coordY->setText(QString("Y: %1").arg(chartPos.y()));

    // 重新定位到右上角
    QRectF plotArea = m_chart->plotArea();
    qreal right = plotArea.right();
    qreal top = plotArea.top();
    m_coordX->setPos(right - m_coordX->boundingRect().width() - 10, top + 10);
    m_coordY->setPos(right - m_coordY->boundingRect().width() - 10,
                     top + 10 + m_coordX->boundingRect().height() + 5);
}

void BaseChartWidget::clearChartData() {
    m_series->clear();
    m_scatterSeries->clear();
}

void BaseChartWidget::setAxisLabels(const QString &xLabel, const QString &yLabel) {
    m_axisX->setTitleText(xLabel);
    m_axisY->setTitleText(yLabel);
}

void BaseChartWidget::setAxisPrecision(int xPrecision, int yPrecision) {
    m_xPrecision = xPrecision;
    m_yPrecision = yPrecision;
    m_axisX->setLabelFormat(QString("%.%1f").arg(m_xPrecision));
    m_axisY->setLabelFormat(QString("%.%1f").arg(m_yPrecision));
}

void BaseChartWidget::setXAxisRange(double min, double max) {
    m_axisX->setRange(min, max);
}

void BaseChartWidget::setYAxisRange(double min, double max) {
    m_axisY->setRange(min, max);
}

void BaseChartWidget::setChartTitle(const QString &title) {
    m_chart->setTitle(title);
}

void BaseChartWidget::setLineColor(const QColor &color) {
    QPen pen = m_series->pen();
    pen.setColor(color);
    m_series->setPen(pen);
}

void BaseChartWidget::setBackgroundColor(const QColor &color) {
    m_chart->setBackgroundBrush(QBrush(color));
}

void BaseChartWidget::setGridVisible(bool visible) {
    m_axisX->setGridLineVisible(visible);
    m_axisY->setGridLineVisible(visible);
}

void BaseChartWidget::resetView() {
    m_axisX->setRange(m_recoverXMin, m_recoverXMax);
    m_axisY->setRange(m_recoverYMin, m_recoverYMax);
}

void BaseChartWidget::autoAdjust() {
    optimizeAxisRanges(m_series->points());
}

void BaseChartWidget::setPeakData(QVector<QPointF> Peaks) {
    mPeaks = Peaks;
}

void BaseChartWidget::markPeak(int index) {
    findPoint(index);
}

void BaseChartWidget::setPointMarker(QPointF point) {
    findPoint(point);
}

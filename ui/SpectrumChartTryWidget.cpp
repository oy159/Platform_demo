#include "SpectrumChartTryWidget.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <cmath>

SpectrumChartTryWidget::SpectrumChartTryWidget(QWidget *parent)
    : BaseChartWidget(parent),
      m_sampleRate(1e8),
      m_frequencyMode(false),
      m_sampleNum(0),
      m_peakCnt(0)
{
    // 设置频谱图特有的默认参数
    setAxisLabels("Frequency (MHz)", "Amplitude (dB)");
    setAxisPrecision(3, 2);
    setLineColor(Qt::cyan);
    // setBackgroundColor(QColor(30, 30, 40));
    setGridVisible(true);
    
    // 初始化峰值标记系列
    m_peakSeries = new QScatterSeries();
    m_peakSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_peakSeries->setMarkerSize(10.0);
    m_peakSeries->setColor(Qt::red);
    m_peakSeries->setBorderColor(Qt::white);
    m_peakSeries->setPen(QPen(Qt::white, 1));
    m_chart->addSeries(m_peakSeries);
    m_peakSeries->attachAxis(m_axisX);
    m_peakSeries->attachAxis(m_axisY);
    
}

void SpectrumChartTryWidget::setSampleRate(double rate) {
    m_sampleRate = rate;
    setFrequencyMode(true);
}

void SpectrumChartTryWidget::setFrequencyMode(bool enable) {
    m_frequencyMode = enable;
    if (m_frequencyMode) {
        m_axisX->setTitleText("Frequency (MHz)");
        m_axisX->setLabelFormat("%.3f");
        // 修改x轴数据为频率
        double freqStep = (m_sampleRate / 1e6) / (2 * m_sampleNum);
        for (int i = 0; i < chart_data.size(); ++i) {
            chart_data[i].setX(i * freqStep);
        }
        m_axisX->setLabelFormat(QString("%.%1f").arg(m_xPrecision));
        updateChartData(chart_data);
    } else {
        m_axisX->setTitleText("Sample Point");
        m_axisX->setLabelFormat("%.0f");
        // 修改x轴数据为采样点
        for (int i = 0; i < chart_data.size(); ++i) {
            chart_data[i].setX(i);
        }
        updateChartData(chart_data);
    }
}

void SpectrumChartTryWidget::handleRefreshPeakData(const std::vector<Peak>& peaks) {
    m_peaks = peaks;
    // peaks 默认是x轴为点数
    if (!m_peaks.empty()) {
        qDebug() << "Received peaks:" << m_peaks[0].position << m_peaks[0].value;
    }

    m_peakSeries->clear();
    m_peakCnt = 0;
}

void SpectrumChartTryWidget::handleRefreshSpectrum(std::vector<double> fft_data) {
    m_sampleNum = fft_data.size();

    // 计算噪声底和平均值
    double sum = 0.0;
    double minValue = std::numeric_limits<double>::max();
    for (double value : fft_data) {
        sum += value;
        if (value < minValue) minValue = value;
    }


    // 准备图表数据
    chart_data.clear();
    if (m_frequencyMode) {
        double freqStep = (m_sampleRate / 1e6) / (2 * fft_data.size());
        for (size_t i = 0; i < fft_data.size(); ++i) {
            chart_data.append(QPointF(i * freqStep, fft_data[i]));
        }
    } else {
        for (size_t i = 0; i < fft_data.size(); ++i) {
            chart_data.append(QPointF(i, fft_data[i]));
        }
    }

    // 更新图表数据
    updateChartData(chart_data);
    
    // 如果已有峰值数据，重新标记
//    if (!m_peaks.empty()) {
//        handleFindPeak();
//    }
}

void SpectrumChartTryWidget::handleFindPeak() {
    if (m_peaks.empty()) return;
    
    m_peakSeries->clear();
    m_peakCnt = 0;
    markPeak(0);
}

void SpectrumChartTryWidget::handleFindNextPeak() {
    if (m_peaks.empty()) return;
    
    m_peakCnt++;
    if (m_peakCnt >= m_peaks.size()) {
        m_peakCnt = 0; // 重置计数器
    }
    markPeak(m_peakCnt);
}

void SpectrumChartTryWidget::markPeak(int index) {
    if (index < 0 || index >= static_cast<int>(m_peaks.size())) return;
    
    double x, y;
    const Peak& peak = m_peaks[index];
    
    if (m_frequencyMode) {
        x = (static_cast<double>(peak.position) / m_sampleNum / 2) * (m_sampleRate / 1e6);
    } else {
        x = peak.position;
    }
    y = peak.value;
    
    m_peakSeries->clear();
    m_peakSeries->append(x, y);
    updateCoordText(QPointF(x, y));
    
    // 添加标注
    if (!m_peakCallout) {
        m_peakCallout = new Callout(m_chart);
    }
    m_peakCallout->setText(QString("Peak %1\n%2 MHz\n%3 dB")
                          .arg(index+1)
                          .arg(x, 0, 'f', 3)
                          .arg(y, 0, 'f', 2));
    m_peakCallout->setAnchor(QPointF(x, y));
    m_peakCallout->setZValue(12);
    m_peakCallout->updateGeometry();
    m_peakCallout->show();
}

void SpectrumChartTryWidget::optimizeAxisRanges(const QVector<QPointF> &data) {
    if (data.isEmpty()) return;
    
    double minX = data.first().x();
    double maxX = data.last().x();
    double minY = data.first().y();
    double maxY = data.last().y();
    
    for (const auto &point : data) {
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }
    
    // 添加10%边距
    double yMargin = qMax(0.1, (maxY - minY) * 0.1);
    
    // 记录恢复范围
    setRecoverRange(minX, maxX, minY - yMargin, maxY + yMargin);
    
    m_axisX->setRange(minX, maxX);
    m_axisY->setRange(minY - yMargin, maxY + yMargin);
}

void SpectrumChartTryWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left) {
        // 向左查找峰值
        if (m_peakCnt > 0) {
            m_peakCnt--;
        } else if (!m_peaks.empty()) {
            m_peakCnt = m_peaks.size() - 1;
        }
        markPeak(m_peakCnt);
        event->accept();
    } else if (event->key() == Qt::Key_Right) {
        // 向右查找峰值
        handleFindNextPeak();
        event->accept();
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_M) {
        // 切换频率模式
        setFrequencyMode(!m_frequencyMode);
        if (!m_peaks.empty()) {
            markPeak(m_peakCnt);
        }
        event->accept();
    } else {
        BaseChartWidget::keyPressEvent(event);
    }
}

void SpectrumChartTryWidget::addCustomContextMenuActions(QMenu *menu) {
    QAction *freqModeAction = menu->addAction("Frequency Mode");
    freqModeAction->setCheckable(true);
    freqModeAction->setChecked(m_frequencyMode);
    connect(freqModeAction, &QAction::toggled, this, &SpectrumChartTryWidget::setFrequencyMode);

    menu->addSeparator();
    QAction *findPeakAction = menu->addAction("Find First Peak");
    connect(findPeakAction, &QAction::triggered, this, &SpectrumChartTryWidget::handleFindPeak);

    QAction *nextPeakAction = menu->addAction("Find Next Peak");
    connect(nextPeakAction, &QAction::triggered, this, &SpectrumChartTryWidget::handleFindNextPeak);

    menu->addSeparator();
}
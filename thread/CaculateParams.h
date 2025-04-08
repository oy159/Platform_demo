//
// Created by oycr on 2025/4/4.
//

#ifndef PLATFORM_DEMO_CACULATEPARAMS_H
#define PLATFORM_DEMO_CACULATEPARAMS_H

#include "QObject"
#include "fftw3.h"
#include <vector>
#include <cmath>
#include "qdebug.h"

struct Peak {
    size_t position;
    double value;
};


class CaculateParams: public QObject {
    Q_OBJECT

public:
    explicit CaculateParams(QObject *parent = nullptr);
    ~CaculateParams();

    double getSFDR() const { return sfdr_db; }
    double getTHD() const { return THD; }
    double getSNR() const { return SNR; }
    double getENOB() const { return ENOB; }

signals:
    void paramsCalculateFinished(double SFDR, double THD, double SNR, double ENOB);
    void TransferFFTData(const std::vector<double>& fftData);

public slots:
    void setData(const std::vector<double>& data) {
        mData = data;
    }
    void calculateParams();

private:
    void calculateFFT();
    std::vector<double> mData;
    std::vector<double> fft_abs;

    std::vector<Peak> findPeaks(const std::vector<double>& data,
                                double minHeight = -INFINITY,
                                size_t minDistance = 10);

    void calculateSFDRdb(int numHarmonics = 5, double dcExcludeWidth = -1);

    double sample_rate = 5e8;

    int fund_peakIndex = 0;
    int exulude_len;

    Peak fund_peak = {0,0};
    Peak spur_peak = {0,0};

    double fund_amp = 0;
    double fund_freq = 0;

    double spur_peakIndex = 0;
    double spur_freq = 0;
    double spur_amp = 0;

    double dc_amp = 0;

    double fund_energy;
    double noise_energy;
    double harmonic_energy;

    double sfdr_db;
    double THD;
    double SNR;
    double ENOB;
};


#endif //PLATFORM_DEMO_CACULATEPARAMS_H

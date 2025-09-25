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

typedef enum {
    Hanning,
    Hamming,
    Rectangular
}WindowFuncClass;


class CaculateParams: public QObject {
    Q_OBJECT

public:
    explicit CaculateParams(QObject *parent = nullptr);
    ~CaculateParams();

    double getSFDR() const { return sfdr_db; }
    double getTHD() const { return THD; }
    double getSNR() const { return SNR; }
    double getENOB() const { return ENOB; }
    void setSampleRate(int sampleRate) {
        sample_rate = sampleRate;
    };
    void setADCBits(int bits) {
        ADC_BITS = bits;
        if (ADC_BITS == 16) {
            // 16位ADC
            HsineWave.resize(65536, 0);
            LSBCodeWidth.resize(65536, 0);
            DNL.resize(65536, 0);
            INL.resize(65536, 0);
        } else {
            // 12位ADC
            HsineWave.resize(4096, 0);
            LSBCodeWidth.resize(4096, 0);
            DNL.resize(4096, 0);
            INL.resize(4096, 0);
        }
    }

signals:
    void dynamicParamsCalculateFinished(double SFDR, double THD, double SNR, double ENOB);
    void TransferFFTData(const std::vector<double>& fftData);
    void TransferPeakData(const std::vector<Peak>& peaks);
    void TransferADCData(const std::vector<double>& data);

    void staticParamsCalculateFinished(double maxDNL, double maxINL, 
                                       double minDNL, double minINL,
                                       double staticOffset, double staticPeak);
    void TransferDNLData(const std::vector<double>& DNL);
    void TransferINLData(const std::vector<double>& INL);
    void TransferHistrogramData(const std::vector<double>& histogramData);

    void TransferDACStaticData(const std::vector<double>& data);

    void staticDACParamsCalculateFinished(double maxDNL, double maxINL, 
                                       double minDNL, double minINL,
                                       double offset, double gain);
    void TransferDACDNLData(const std::vector<double>& DNL);
    void TransferDACINLData(const std::vector<double>& INL);

public slots:
    void setData(const std::vector<double>& data) {
        mData = data;
        emit TransferADCData(data);
    }
    void caculateDynamicParamsADC();
    void caculateStaticParamsADC();
    void caculateStaticDataHistogram();

    void setDACStaticData(const std::vector<double>& data) {
        dacStaticData = data;
        emit TransferDACStaticData(data);
    }
    void caculateDACStaticParams();
    void setWindowFunc(WindowFuncClass windowFunc);
    void calculateADCTwoToneParams(double f1, double f2);

private:
    void calculateFFT();
    static int findPeakNearFrequency(double target_freq, const std::vector<double>& freq, const std::vector<double>& fft_db);
    std::vector<double> mData;
    std::vector<double> fft_abs;

    std::vector<Peak> findPeaks(const std::vector<double>& data,
                                double minHeight = -INFINITY,
                                size_t minDistance = 10);

    void calculateSFDRdb(int numHarmonics = 5, double dcExcludeWidth = -1);

    double sample_rate = 5e8;

    int fund_peakIndex = 0;

    std::vector<Peak> fft_db_peaks;

    double fund_amp = 0;
    double fund_freq = 0;
    int fund_bin = 0;

    double spur_amp = 0;

    double fund_energy;
    double noise_energy;
    double harmonic_energy;

    double sfdr_db;
    double THD;
    double SNR;
    double ENOB;

    std::vector<uint32_t> StaticDataHistogram;
    uint32_t ADCStaticDataLength = 0;

    int ADC_BITS = 12; // 假设ADC是12位的

    double staticOffset = 0.0;
    double staticPeak = 0.0;
    std::vector<double> HsineWave; // 理想正弦波分布
    std::vector<double> LSBCodeWidth; // LSB代码宽度
    std::vector<double> DNL; // 差分非线性
    std::vector<double> INL; // 积分非线性
    double maxDNL = 0.0;
    double maxINL = 0.0;
    double minINL = 0.0;
    double minDNL = 0.0;


    std::vector<double> dacStaticData;
    std::vector<double> DACDNL; // 差分非线性
    std::vector<double> DACINL; // 积分非线性
    double maxDACDNL = 0.0;
    double maxDACINL = 0.0;
    double minDACINL = 0.0;
    double minDACDNL = 0.0;

    double ADC_IMD3 = 0.0;

    WindowFuncClass windowFunc = Hanning;

};


#endif //PLATFORM_DEMO_CACULATEPARAMS_H

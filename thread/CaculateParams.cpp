//
// Created by oycr on 2025/4/4.
//

#include "CaculateParams.h"

CaculateParams::CaculateParams(QObject *parent) : QObject(parent)  {

}

CaculateParams::~CaculateParams() {
    // Destructor implementation
}

void CaculateParams::calculateFFT() {
    // FFT calculation implementation
    int N = mData.size();
    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    for (int i = 0; i < N; ++i) {
        double window = 0.5 * (1 - cos(2 * M_PI * i / (N - 1))); // 汉宁窗
        in[i][0] = mData[i] * window; // Real part
        in[i][1] = 0.0;     // Imaginary part
    }

    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p); // Execute the FFT

    // Calculate the absolute value of the FFT result
    fft_abs.clear();
    for (int i = 0; i < N/2; ++i) {
        fft_abs.push_back(sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]));
    }

    double max_magnitude = 0.0;
    for (int i = 0; i < N / 2; ++i) {  // 只需检查前一半（对称性）
        if (fft_abs[i] > max_magnitude) {
            max_magnitude = fft_abs[i];
            fund_peakIndex = i;
        }
    }

    double max_input = 0.0;
    for (int i = 0; i < N; ++i) {  // 只需检查前一半（对称性）
        if (mData[i] > max_input) {
            max_input = mData[i];
        }
    }

    qDebug() << max_input;
    // Clean up
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

void CaculateParams::caculateDynamicParamsADC() {
    calculateFFT();

    // Calculate sfdr_db, THD, SNR, ENOB
    double max_magnitude = fft_abs[fund_peakIndex];
    auto N = fft_abs.size();

   
//
//    fund_peak = peaks[0];

    calculateSFDRdb();

    THD = 10 * log10(harmonic_energy / (fund_energy));

// 4. 计算 SNR 和 ENOB
    SNR = 10 * log10((fund_energy)/ noise_energy);
    double SNDR = 10 * log10((fund_energy)/(noise_energy + harmonic_energy));
    ENOB = (SNDR - 1.76) / 6.02;

    emit paramsCalculateFinished(sfdr_db, THD, SNR, ENOB);

    std::vector<double> fft_db;
    fft_db.reserve(fft_abs.size());
    for (double fft_ab : fft_abs) {
        fft_db.push_back(20 * log10(fft_ab / max_magnitude));
    }

    fft_db_peaks.clear();
    fft_db_peaks = findPeaks(fft_db, -INFINITY, 20);
    std::sort(fft_db_peaks.begin(), fft_db_peaks.end(),
             [](const Peak& a, const Peak& b) { return a.value > b.value; });

    emit TransferFFTData(fft_db);
    emit TransferPeakData(fft_db_peaks);
}



std::vector<Peak> CaculateParams::findPeaks(const std::vector<double>& data,
                    double minHeight,
                    size_t minDistance){
    std::vector<Peak> peaks;

    if (data.size() < 3) return peaks;

    // 第一步：找到所有候选峰值
    for (size_t i = 1; i < data.size() - 1; ++i) {
        if (data[i] > data[i-1] && data[i] > data[i+1] && data[i] > minHeight) {
            peaks.push_back({i, data[i]});
        }
    }

    // 第二步：根据最小距离筛选峰值
    if (minDistance > 1 && !peaks.empty()) {
        std::vector<Peak> filteredPeaks;
        filteredPeaks.push_back(peaks[0]);

        for (size_t i = 1; i < peaks.size(); ++i) {
            if (peaks[i].position - filteredPeaks.back().position >= minDistance) {
                filteredPeaks.push_back(peaks[i]);
            } else if (peaks[i].value > filteredPeaks.back().value) {
                filteredPeaks.back() = peaks[i]; // 保留更高的峰值
            }
        }

        return filteredPeaks;
    }

    return peaks;
}

void CaculateParams::calculateSFDRdb(int numHarmonics, double dcExcludeWidth) {

    // 检查输入有效性
    if (fft_abs.empty()) return;

    // 设置默认直流排除带宽为fs/1000
    if (dcExcludeWidth <= 0) {
        dcExcludeWidth = sample_rate / 1000.0;
    }

    auto nfft = (fft_abs.size()) * 2; // 计算原始FFT点数
    const auto& fft_mag = fft_abs;        // 引用输入数据
    double max_mag = *std::max_element(fft_mag.begin(), fft_mag.end());

    // 计算频率向量
    std::vector<double> freq(fft_mag.size());
    for (size_t i = 0; i < freq.size(); ++i) {
        freq[i] = (double)i * sample_rate / (double)nfft;
    }

    // 计算直流分量区域
    int dc_bin_width = static_cast<int>(ceil(dcExcludeWidth * nfft / sample_rate));
    dc_bin_width = std::min(dc_bin_width, static_cast<int>(fft_mag.size()) - 1);

    // 计算直流分量幅度
    double dc_sum = 0.0;
    for (int i = 0; i <= dc_bin_width; ++i) {
        dc_sum += fft_mag[i];
    }
    dc_amp = 20 * log10(dc_sum / (dc_bin_width + 1) / max_mag);

    // 寻找基波频率（跳过直流区域）
    int search_start = dc_bin_width + 1;
    auto fund_bin = search_start;
    for (size_t i = search_start; i < fft_mag.size(); ++i) {
        if (fft_mag[i] > fft_mag[fund_bin]) {
            fund_bin = static_cast<int>(i);
        }
    }
    fund_freq = freq[fund_bin];
    fund_amp = 20 * log10(fft_mag[fund_bin] / max_mag);

    // 定义基波排除区域（±8%）
    double exclude_band = 0.08 * fund_freq;
    int exclude_start = std::max(1, static_cast<int>(floor((fund_freq - exclude_band) * nfft / sample_rate)));
    int exclude_end = std::min(static_cast<int>(fft_mag.size()) - 1,
                               static_cast<int>(ceil((fund_freq + exclude_band) * nfft / sample_rate)));

    exulude_len = (exclude_end - exclude_start + 1)/2;

    // 考虑谐波区域
    fund_energy = 0;
    std::vector<bool> exclude_bins(fft_mag.size(), false);
    for (int i = exclude_start; i <= exclude_end; ++i) {
        exclude_bins[i] = true;
        fund_energy += fft_mag[i] * fft_mag[i];
    }

    harmonic_energy = 0;
    for (int k = 2; k <= numHarmonics; ++k) {
        double harmonic_freq = k * fund_freq;
        if (harmonic_freq < sample_rate/2) {
            int h_start = std::max(1, static_cast<int>(floor((harmonic_freq - exclude_band) * nfft / sample_rate)));
            int h_end = std::min(static_cast<int>(fft_mag.size()) - 1,
                                 static_cast<int>(ceil((harmonic_freq + exclude_band) * nfft / sample_rate)));
            for (int i = h_start; i <= h_end; ++i) {
//                exclude_bins[i] = true;
                harmonic_energy += fft_mag[i] * fft_mag[i];
            }
        }
    }

    // 排除直流区域
    for (int i = 0; i <= dc_bin_width; ++i) {
        exclude_bins[i] = true;
    }

    // 寻找最大杂散
    spur_amp = -std::numeric_limits<double>::infinity();
    spur_freq = NAN;
    auto spur_bin = -1;
    noise_energy = 0.0;

    for (size_t i = 1; i < fft_mag.size(); ++i) {
        if (!exclude_bins[i] && fft_mag[i] > spur_amp) {
            spur_amp = fft_mag[i];
            spur_bin = static_cast<int>(i);
            spur_freq = freq[i];
        }if(!exclude_bins[i]){
            noise_energy += fft_mag[i] * fft_mag[i];
        }
    }
    spur_peakIndex = spur_bin;

    if (spur_amp > 0) {
        spur_amp = 20 * log10(spur_amp / max_mag);
        sfdr_db = fund_amp - spur_amp;
    } else {
        spur_amp = -std::numeric_limits<double>::infinity();
        sfdr_db = std::numeric_limits<double>::infinity();
    }
}

void CaculateParams::caculateStaticParamsADC() {

}

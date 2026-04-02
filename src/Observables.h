#ifndef OBSERVABLES_H
#define OBSERVABLES_H

/**
 * 可观测量统计类
 * 用于收集和分析蒙特卡洛模拟中的物理量
 */
class Observables {
private:
    int nSamples;           // 样本数量
    double sumM;            // 磁化强度累加
    double sumM2;           // 磁化强度平方累加
    double sumE;            // 能量累加
    double sumE2;           // 能量平方累加

public:
    Observables();

    void reset();                                   // 重置统计量
    void addSample(double M, double E);             // 添加测量样本

    // 统计分析
    double getMeanMagnetization() const;            // 平均磁化强度
    double getMeanEnergy() const;                   // 平均能量
    double getMagneticSusceptibility(double beta, int N) const;  // 磁化率
    double getSpecificHeat(double beta, int N) const;            // 比热

    int getSampleCount() const { return nSamples; }
};

#endif // OBSERVABLES_H
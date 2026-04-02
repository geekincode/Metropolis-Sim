#include "Observables.h"

/**
 * 构造函数：初始化统计量
 */
Observables::Observables() {
    reset();
}

/**
 * 重置所有统计量
 */
void Observables::reset() {
    nSamples = 0;
    sumM = 0.0;
    sumM2 = 0.0;
    sumE = 0.0;
    sumE2 = 0.0;
}

/**
 * 添加一个测量样本
 */
void Observables::addSample(double M, double E) {
    nSamples++;
    sumM += M;
    sumM2 += M * M;
    sumE += E;
    sumE2 += E * E;
}

/**
 * 计算平均磁化强度
 */
double Observables::getMeanMagnetization() const {
    if (nSamples == 0) return 0.0;
    return sumM / nSamples;
}

/**
 * 计算平均能量
 */
double Observables::getMeanEnergy() const {
    if (nSamples == 0) return 0.0;
    return sumE / nSamples;
}

/**
 * 计算磁化率 χ = β*N*(⟨M²⟩ - ⟨M⟩²)
 */
double Observables::getMagneticSusceptibility(double beta, int N) const {
    if (nSamples == 0) return 0.0;

    double meanM = sumM / nSamples;
    double meanM2 = sumM2 / nSamples;
    return beta * N * (meanM2 - meanM * meanM);
}

/**
 * 计算比热 C_v = β²*N*(⟨E²⟩ - ⟨E⟩²)
 */
double Observables::getSpecificHeat(double beta, int N) const {
    if (nSamples == 0) return 0.0;

    double meanE = sumE / nSamples;
    double meanE2 = sumE2 / nSamples;
    return beta * beta * (meanE2 - meanE * meanE) / N;
}
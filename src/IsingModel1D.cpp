#include "IsingModel1D.h"
#include <cmath>
#include <iostream>

/**
 * 构造函数：初始化一维 Ising 模型
 */
IsingModel1D::IsingModel1D(int size, double coupling, double temperature, unsigned int seed)
    : N(size), J(coupling), T(temperature), beta(1.0 / temperature),
      rng(seed), site_dist(0, size - 1), prob_dist(0.0, 1.0) {

    spins.resize(N);
    randomizeSpins();
}

/**
 * 随机初始化自旋配置
 */
void IsingModel1D::randomizeSpins() {
    std::uniform_int_distribution<int> spin_dist(0, 1);
    for (int i = 0; i < N; ++i) {
        spins[i] = 2 * spin_dist(rng) - 1;  // 随机生成 +1 或 -1
    }
}

/**
 * 计算翻转某个格点自旋后的能量变化
 * 使用周期性边界条件
 */
double IsingModel1D::calculateEnergyChange(int site) const {
    int left = (site - 1 + N) % N;   // 左邻居（周期性边界）
    int right = (site + 1) % N;      // 右邻居（周期性边界）
    int neighborSum = spins[left] + spins[right];

    // ΔE = E_new - E_old = 2 * J * σ_i * (σ_{i-1} + σ_{i+1})
    // （因为哈密顿量定义为 H = -J * sum(σ_i * σ_{i+1})）
    return 2.0 * J * spins[site] * neighborSum;
}

/**
 * 单次 Metropolis 步骤：尝试翻转一个随机选择的自旋
 */
void IsingModel1D::metropolisStep() {
    // 随机选择一个格点
    int site = site_dist(rng);

    // 计算能量变化
    double dE = calculateEnergyChange(site);

    // Metropolis 接受准则
    if (dE <= 0.0) {
        // ΔE ≤ 0，总是接受（降低能量）
        spins[site] *= -1;
    } else {
        // ΔE > 0，以概率 exp(-β*ΔE) 接受
        double acceptProb = std::exp(-beta * dE);
        if (prob_dist(rng) < acceptProb) {
            spins[site] *= -1;
        }
    }
}

/**
 * 一次扫描：执行 N 次 Metropolis 步骤
 */
void IsingModel1D::metropolisSweep() {
    for (int i = 0; i < N; ++i) {
        metropolisStep();
    }
}

/**
 * 平衡化阶段：运行多次扫描使系统达到热平衡
 */
void IsingModel1D::equilibrate(int nSweeps) {
    for (int sweep = 0; sweep < nSweeps; ++sweep) {
        metropolisSweep();
    }
}

/**
 * 计算磁化强度 M = (1/N) * Σσ_i
 */
double IsingModel1D::getMagnetization() const {
    int totalSpin = 0;
    for (int s : spins) {
        totalSpin += s;
    }
    return static_cast<double>(totalSpin) / N;
}

/**
 * 计算系统总能量 E = -J * Σσ_i*σ_{i+1}
 */
double IsingModel1D::getEnergy() const {
    double energy = 0.0;
    for (int i = 0; i < N; ++i) {
        int next = (i + 1) % N;  // 周期性边界条件
        energy -= J * spins[i] * spins[next];
    }
    return energy;
}

/**
 * 设置新温度并更新 beta
 */
void IsingModel1D::setTemperature(double newT) {
    T = newT;
    beta = 1.0 / T;
}

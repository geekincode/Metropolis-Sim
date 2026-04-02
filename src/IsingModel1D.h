#ifndef ISINGMODEL1D_H
#define ISINGMODEL1D_H

#include <vector>
#include <random>

/**
 * 一维 Ising 模型类
 * 使用 Metropolis 蒙特卡洛方法模拟一维自旋链
 */
class IsingModel1D {
private:
    int N;                                          // 系统大小（自旋数量）
    std::vector<int> spins;                         // 自旋配置 (+1 或 -1)
    double J;                                       // 耦合常数
    double T;                                       // 温度
    double beta;                                    // 逆温度 β = 1/(k_B*T)

    // 随机数生成器
    std::mt19937 rng;
    std::uniform_int_distribution<int> site_dist;
    std::uniform_real_distribution<double> prob_dist;

    // 能量计算辅助函数
    double calculateEnergyChange(int site) const;

public:
    /**
     * 构造函数
     * @param size 系统大小
     * @param coupling 耦合常数 J
     * @param temperature 温度 T
     * @param seed 随机数种子
     */
    IsingModel1D(int size, double coupling, double temperature, unsigned int seed);

    // 核心蒙特卡洛方法
    void metropolisStep();                          // 单次 Metropolis 步骤
    void metropolisSweep();                         // 一次扫描（N 次步骤）
    void equilibrate(int nSweeps);                  // 平衡化阶段

    // 测量方法
    double getMagnetization() const;                // 计算磁化强度
    double getEnergy() const;                       // 计算系统能量
    int getSystemSize() const { return N; }         // 获取系统大小

    // 配置管理
    void randomizeSpins();                          // 随机初始化自旋
    void setTemperature(double newT);               // 设置新温度
};

#endif // ISINGMODEL1D_H

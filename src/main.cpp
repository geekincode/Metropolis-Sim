#include "IsingModel1D.h"
#include "Observables.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <vector>

int main() {
    // ========== 模拟参数 ==========
    std::vector<int> systemSizes = {100, 400, 900};  // 不同系统尺寸
    const double J = 5.0;               // 耦合常数
    const int nEquil = 500000;            // 平衡化扫描次数
    const int nMeasure = 1000000;         // 测量扫描次数
    const int measureInterval = 2;     // 测量间隔

    // 温度范围
    const double T_min = 0.1;
    const double T_max = 5.0;
    const double T_step = 0.1;

    // 随机数种子
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::cout << "========================================\n"
              << "  一维 Ising 模型 Metropolis MC 模拟\n"
              << "  多系统尺寸对比研究\n"
              << "========================================\n";
    std::cout << "系统尺寸: N = ";
    for (size_t i = 0; i < systemSizes.size(); ++i) {
        std::cout << systemSizes[i];
        if (i < systemSizes.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";
    std::cout << "耦合常数: J = " << J << "\n";
    std::cout << "温度范围: T = " << T_min << " ~ " << T_max << "\n";
    std::cout << "平衡化扫描: " << nEquil << "\n";
    std::cout << "测量扫描: " << nMeasure << "\n";
    std::cout << "========================================\n\n";

    // 获取项目根目录（假设可执行文件在 build/ 目录中）
    std::filesystem::path exePath = std::filesystem::current_path();
    std::filesystem::path projectRoot;

    // 检查当前路径，确定项目根目录
    if (exePath.filename() == "build") {
        projectRoot = exePath.parent_path();
    } else {
        projectRoot = exePath;
    }

    std::filesystem::path dataDir = projectRoot / "data";

    // 确保 data 目录存在
    std::filesystem::create_directories(dataDir);

    // ========== 对每个系统尺寸进行模拟 ==========
    for (int N : systemSizes) {
        std::cout << "\n>>> 开始模拟 N = " << N << " <<<\n\n";

        // 为每个尺寸创建单独的数据文件
        std::string suffix = "_N" + std::to_string(N) + ".dat";
        std::ofstream magFile(dataDir / ("magnetization" + suffix));
        std::ofstream chiFile(dataDir / ("susceptibility" + suffix));
        std::ofstream energyFile(dataDir / ("energy" + suffix));

        if (!magFile || !chiFile || !energyFile) {
            std::cerr << "错误：无法创建输出文件！\n";
            continue;
        }

        // 写入文件头
        magFile << "# Temperature  Magnetization (N=" << N << ")\n";
        chiFile << "# Temperature  Susceptibility (N=" << N << ")\n";
        energyFile << "# Temperature  Energy (N=" << N << ")\n";

        // 温度扫描
        int tempCount = 0;
        for (double T = T_min; T <= T_max; T += T_step) {
            tempCount++;

            // 创建 Ising 模型实例
            IsingModel1D model(N, J, T, seed + tempCount + N);
            Observables obs;

            // 平衡化阶段
            std::cout << "T = " << std::fixed << std::setprecision(2) << T
                      << " - 平衡化中..." << std::flush;
            model.equilibrate(nEquil);

            // 测量阶段
            std::cout << " 测量中..." << std::flush;
            for (int sweep = 0; sweep < nMeasure; ++sweep) {
                model.metropolisSweep();

                // 每隔一定步数测量一次
                if (sweep % measureInterval == 0) {
                    double M = std::abs(model.getMagnetization());  // 取绝对值
                    double E = model.getEnergy();
                    obs.addSample(M, E);
                }
            }

            // 计算统计量
            double meanM = obs.getMeanMagnetization();
            double meanE = obs.getMeanEnergy();
            double chi = obs.getMagneticSusceptibility(1.0 / T, N);

            // 写入数据文件
            magFile << T << "  " << meanM << "\n";
            chiFile << T << "  " << chi << "\n";
            energyFile << T << "  " << meanE / N << "\n";

            std::cout << " 完成 (M=" << std::setprecision(4) << meanM
                      << ", χ=" << std::setprecision(2) << chi << ")\n";
        }

        // 关闭文件
        magFile.close();
        chiFile.close();
        energyFile.close();

        std::cout << "N = " << N << " 模拟完成！\n";
    }

    std::cout << "\n========================================\n";
    std::cout << "所有模拟完成！\n";
    std::cout << "数据已保存到: " << projectRoot / "data" << "\n";
    std::cout << "请运行以下命令生成图表：\n";
    std::cout << "  cd " << projectRoot << " && gnuplot scripts/plot.gnu\n";
    std::cout << "========================================\n";

    return 0;
}

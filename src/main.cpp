#include "IsingModel1D.h"
#include "Observables.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <vector>

#include <mpi.h>

int main(int argc, char** argv) {
    // 初始化MPI
    MPI_Init(&argc, &argv);

    // 获取当前进程的rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // 获取总的进程数
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 获取处理器名称
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

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

    if (rank == 0) {
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
    }

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

    // ========== MPI 任务分配 ==========
    int numTasks = systemSizes.size();
    int tasksPerProcess = (numTasks + size - 1) / size;  // 向上取整

    auto start_time = std::chrono::high_resolution_clock::now();

    if (rank == 0) {
        std::cout << "\n[MPI] 总进程数: " << size << "\n";
        std::cout << "[MPI] 系统尺寸数: " << numTasks << "\n";
        std::cout << "[MPI] 每个进程平均任务数: " << tasksPerProcess << "\n";
        std::cout << "========================================\n\n";
    }

    // 确保在每个进程上创建输出目录
    std::filesystem::create_directories(dataDir);

    // 每个进程处理分配给它的系统尺寸
    int startIdx = rank * tasksPerProcess;
    int endIdx = std::min((rank + 1) * tasksPerProcess, numTasks);

    // ========== 对分配的系统尺寸进行模拟 ==========
    for (int idx = startIdx; idx < endIdx; ++idx) {
        int N = systemSizes[idx];

        if (rank == 0 || size > 1) {
            std::cout << "[rank " << rank << "] 开始模拟 N = " << N << std::endl;
        }

        // 为每个尺寸创建单独的数据文件
        std::string suffix = "_N" + std::to_string(N) + ".dat";
        std::ofstream magFile(dataDir / ("magnetization" + suffix));
        std::ofstream chiFile(dataDir / ("susceptibility" + suffix));
        std::ofstream energyFile(dataDir / ("energy" + suffix));

        if (!magFile || !chiFile || !energyFile) {
            std::cerr << "[rank " << rank << "] 错误：无法创建输出文件！\n";
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

            // 创建 Ising 模型实例 (使用rank作为种子的一部分，确保不同进程有不同的随机数)
            IsingModel1D model(N, J, T, seed + tempCount + N + rank * 10000);
            Observables obs;

            // 平衡化阶段
            std::cout << "[rank " << rank << "] T = " << std::fixed
                      << std::setprecision(2) << T << " - 平衡化中..."
                      << std::flush;
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

        if (rank == 0 || size > 1) {
            std::cout << "[rank " << rank << "] N = " << N << " 模拟完成！\n";
        }
    }

    // ========== MPI 同步和最终输出 ==========
    // 所有进程等待完成
    MPI_Barrier(MPI_COMM_WORLD);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    // 只有主进程输出最终信息
    if (rank == 0) {
        std::cout << "\n========================================\n";
        std::cout << "所有模拟完成！\n";
        std::cout << "总耗时: " << duration.count() << " 秒\n";
        std::cout << "数据已保存到: " << dataDir << "\n";
        std::cout << "请运行以下命令生成图表：\n";
        std::cout << "  cd " << projectRoot << " && gnuplot scripts/plot.gnu\n";
        std::cout << "========================================\n";
    }

    // 清理MPI
    MPI_Finalize();

    return 0;
}

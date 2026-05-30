# 一维 Ising 模型  蒙特卡洛模拟

## 项目简介

本项目使用 **Metropolis 蒙特卡洛方法**模拟一维 Ising 模型（一维自旋链），计算并绘制磁化强度和磁化率随温度的变化关系。

### 物理背景

- **一维 Ising 模型**：描述一条自旋链，每个自旋可以向上（+1）或向下（-1）
- **哈密顿量**：H = -J ∑⟨i,j⟩ σᵢσⱼ（J 为耦合常数）
- **关键特性**：一维系统在任何正温度下都没有相变，但展示平滑的交叉行为

### 实现特点

- ✅ 自己实现 Metropolis 蒙特卡洛抽样算法
- ✅ 使用周期性边界条件
- ✅ 高效的能量变化计算（只计算局部变化）
- ✅ 模块化设计，代码清晰易懂
- ✅ 使用 CMake 管理项目

## 快速开始

### 方式 A：MPI 并行运行（推荐）

```bash
# 1. 编译
cd /home/geekincode/Physics/MC
mkdir -p build && cd build
cmake ..
make -j12

# 2. MPI 并行后台运行（4 进程，约 3 分钟）
cd /home/geekincode/Physics/MC
nohup mpirun -n 4 ./build/ising1d > log &

# 3. 监视进度
tail -f log

# 4. 计算完成后生成图表
gnuplot scripts/plot.gnu

# 5. 查看结果
open plots/results.png
```

### 方式 B：串行运行

```bash
# 编译
cd /home/geekincode/Physics/MC
mkdir -p build && cd build
cmake ..
make -j12

# 前台运行（约 10 分钟）
./ising1d

# 生成图表
cd ..
gnuplot scripts/plot.gnu

# 查看结果
open plots/results.png
```

## 项目结构

```
Physics/MC/
├── src/
│   ├── IsingModel1D.h        # Ising 模型类接口
│   ├── IsingModel1D.cpp      # Ising 模型核心实现
│   ├── Observables.h         # 可观测量计算接口
│   ├── Observables.cpp       # 统计分析实现
│   └── main.cpp              # 主程序
├── build/                    # 编译输出目录
│   └── ising1d               # 可执行文件（编译后生成）
├── data/                     # 输出数据
│   ├── magnetization_N*.dat  # 磁化强度数据
│   ├── susceptibility_N*.dat # 磁化率数据
│   └── energy_N*.dat         # 能量数据
├── plots/                    # 生成的图表
│   └── results.png           # 最终绘图结果
├── scripts/
│   └── plot.gnu              # Gnuplot 绘图脚本
├── CLAUDE.md                 # Claude 开发指南
├── CMakeLists.txt            # CMake 配置
├── README.md                 # 本文件
└── log                       # 后台运行日志（使用 nohup 时生成）
```

## 编译和运行

### 前置要求

- C++ 编译器（支持 C++17）
- CMake (>= 3.10)
- MPI（OpenMPI 或 MPICH）
- Gnuplot（用于绘图）

#### 依赖安装

**Ubuntu/Debian：**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake gnuplot libopenmpi-dev
```

**macOS（使用 Homebrew）：**
```bash
brew install cmake gnuplot open-mpi
```

**CentOS/RHEL：**
```bash
sudo yum install gcc-c++ cmake gnuplot openmpi-devel
```

### 编译步骤

```bash
cd /home/geekincode/Physics/MC
mkdir build && cd build
cmake ..
make
```

### 运行模拟

#### 方式一：MPI 并行运行（推荐）

**前台运行（可实时查看输出）**

```bash
cd /home/geekincode/Physics/MC/build

# 4 个进程并行运行（约 3 分钟）
mpirun -n 4 ./ising1d

# 或 8 个进程（对于 8 核系统，约 2 分钟）
mpirun -n 8 ./ising1d
```

**后台运行（推荐，支持断网继续计算）**

```bash
cd /home/geekincode/Physics/MC

# 4 进程后台运行
nohup mpirun -n 4 ./build/ising1d > log &

# 8 进程后台运行
nohup mpirun -n 8 ./build/ising1d > log &
```

**MPI 进程数选择建议**：
| 系统尺寸数 | 推荐进程数 | 预计耗时 |
|----------|----------|---------|
| 3 (N=100,400,900) | 2-3 | 5-7 分钟 |
| 6 | 4-6 | 2-3 分钟 |
| 9+ | 8+ | 1-2 分钟 |

#### 方式二：串行运行

**前台直接运行**

```bash
cd /home/geekincode/Physics/MC/build
./ising1d
```

模拟将运行约 10 分钟，数据会自动保存到项目根目录的 `data/` 文件夹。

#### 后台进程管理

```bash
# 查看后台进程
jobs
ps aux | grep ising1d
ps aux | grep mpirun

# 实时查看输出日志
tail -f log

# 检查计算进度（查看已生成的数据文件）
ls -lh data/

# 停止后台进程
kill %1          # 根据 jobs 输出的编号
# 或强制停止所有 mpirun 进程
pkill -f mpirun
pkill -f ising1d
```

**说明**：
- `nohup` - 让进程在终端关闭后继续运行
- `> log` - 将标准输出重定向到 `log` 文件
- `&` - 放在后台执行
- `mpirun -n N` - 使用 N 个进程运行（每个进程分配 1-2 个系统尺寸）
- `tail -f log` - 实时监视日志输出（按 Ctrl+C 退出）

### 生成图表

#### 前台运行后生成图表

在模拟完成后，运行：

```bash
cd /home/geekincode/Physics/MC
gnuplot scripts/plot.gnu
open plots/results.png
```

#### 后台运行时生成图表

```bash
# 监视日志，等待计算完成
tail -f log

# 日志显示 "所有模拟完成！" 后，按 Ctrl+C 退出 tail

# 生成图表
gnuplot scripts/plot.gnu

# 查看结果
open plots/results.png
```

#### 快速检查进度

```bash
# 查看已生成的数据文件个数和大小
ls -lh data/

# 实时监视数据文件大小变化
watch -n 5 'ls -lh data/'
```

## 模拟参数

| 参数 | 值 | 说明 |
|------|-----|------|
| **系统尺寸** | N = 100, 400, 900 | 多尺寸对比研究，观察有限尺寸效应 |
| **耦合常数** | J = 5.0 | Ising 模型耦合强度（已优化用于加速收敛） |
| **温度范围** | T = 0.1 ~ 5.0 | 从低温到高温扫描 |
| **温度步长** | ΔT = 0.1 | 温度点数 = 49 |
| **平衡化扫描** | nEquil = 500000 | 热化阶段（数据不计入统计） |
| **测量扫描** | nMeasure = 1000000 | 测量阶段（计入统计） |
| **测量间隔** | 每 2 次扫描 | 采样 500000 个数据点/温度点 |

### 参数调整

修改 `src/main.cpp` 第 31-40 行：

```cpp
std::vector<int> systemSizes = {100, 400, 900};  // 修改系统尺寸
const double J = 5.0;                            // 修改耦合常数
const int nEquil = 500000;                       // 修改平衡化扫描数
const int nMeasure = 1000000;                    // 修改测量扫描数
const int measureInterval = 2;                   // 修改测量间隔
const double T_min = 0.1, T_max = 5.0;          // 修改温度范围
const double T_step = 0.1;                       // 修改温度步长
```


## 核心算法

### Metropolis 接受准则

```cpp
if (ΔE ≤ 0) {
    接受翻转（总是）
} else {
    以概率 exp(-β*ΔE) 接受翻转
}
```

### 能量变化计算

```cpp
ΔE = 2*J*σᵢ*(σᵢ₋₁ + σᵢ₊₁)
```

只计算局部变化，避免重复计算整个系统能量。

## 常见问题

### Q1: MPI 没有安装或 cmake 找不到 MPI？

**症状**：编译时出现 `Could not find MPI` 错误

**解决**：

```bash
# Ubuntu/Debian
sudo apt-get install libopenmpi-dev

# macOS
brew install open-mpi

# CentOS/RHEL
sudo yum install openmpi-devel

# 重新编译
cd build && rm -f CMakeCache.txt && cmake .. && make -j12
```

### Q2: 如何选择合适的进程数？

**建议**：
- **3 个系统尺寸**：2-3 进程（每个进程 1 个任务）
- **6 个系统尺寸**：4-6 进程
- **8+ 个系统尺寸**：8-16 进程

**查询系统核心数**：
```bash
# Linux/macOS
nproc              # 逻辑核心数
sysctl -n hw.ncpu  # macOS 核心数

# 推荐：进程数 = CPU 核心数（或略少）
```

### Q3: MPI 运行和串行运行有什么区别？

| 方式 | 进程数 | 预计耗时 | 适用场景 |
|------|--------|---------|---------|
| 串行 `./ising1d` | 1 | ~10 分钟 | 单核系统或快速测试 |
| MPI 2 进程 | 2 | ~5 分钟 | 双核系统 |
| MPI 4 进程 | 4 | ~3 分钟 | 四核系统（推荐） |
| MPI 8 进程 | 8 | ~2 分钟 | 8+ 核系统 |

### Q4: 后台运行的进程如何确认还在运行？

```bash
# 方法1：查看 MPI 进程
ps aux | grep mpirun
ps aux | grep ising1d

# 方法2：查看日志更新
tail -f log

# 方法3：检查数据文件大小是否在增长
watch -n 5 'ls -lh data/'
```

### Q5: 如何停止正在运行的后台进程？

```bash
# 优雅停止单个任务
kill %1          # 如果用 jobs 看到编号

# 强制杀死所有 mpirun 进程
pkill -f mpirun

# 强制杀死所有 ising1d 进程
pkill -f ising1d
```

### Q6: 计算结果数据在哪里？

所有数据文件都保存在 `data/` 目录下：
```
data/
├── magnetization_N100.dat    # 磁化强度 (N=100)
├── magnetization_N400.dat    # 磁化强度 (N=400)
├── magnetization_N900.dat    # 磁化强度 (N=900)
├── susceptibility_N100.dat   # 磁化率 (N=100)
├── susceptibility_N400.dat   # 磁化率 (N=400)
├── susceptibility_N900.dat   # 磁化率 (N=900)
├── energy_N100.dat           # 能量 (N=100)
├── energy_N400.dat           # 能量 (N=400)
└── energy_N900.dat           # 能量 (N=900)
```

### Q7: 如何加快计算速度？

**方法 1：使用更多进程**
```bash
# 从 4 进程改为 8 进程
nohup mpirun -n 8 ./build/ising1d > log &
```

**方法 2：减小参数（牺牲精度）**
```cpp
// src/main.cpp 第 33-35 行，改为快速配置
const int nEquil = 50000;      // 从 500000 减小到 50000
const int nMeasure = 100000;   // 从 1000000 减小到 100000
const int measureInterval = 10; // 从 2 增大到 10
```

**方法 3：减少系统尺寸或温度点数**
```cpp
// 只模拟两个尺寸
std::vector<int> systemSizes = {100, 900};

// 或减少温度点数
const double T_step = 0.2;     // 从 0.1 增大到 0.2
```

### Q8: Gnuplot 画图失败？

确保安装了 Gnuplot 并且 `scripts/plot.gnu` 文件存在：

```bash
# macOS
brew install gnuplot

# Ubuntu/Debian
sudo apt-get install gnuplot

# CentOS/RHEL
sudo yum install gnuplot

# 检查安装
gnuplot --version

# 手动运行绘图
cd /home/geekincode/Physics/MC
gnuplot scripts/plot.gnu
```

## 性能分析

### 计算加速

使用 MPI 并行化后，每个进程独立计算一个或多个系统尺寸，实现线性加速：

```
系统尺寸 3 个 (N=100, 400, 900)

进程数  | 任务分配        | 预计耗时 | 加速比
--------|------------------|---------|-------
1 进程  | 全部 3 个       | ~10 分钟| 1x
2 进程  | 各 2 个和 1 个   | ~5 分钟 | 2x
3 进程  | 各 1 个         | ~3 分钟 | 3.3x
4 进程  | 3 个有任务      | ~3 分钟 | 3.3x
```

### 内存占用

- **单进程内存**：约 20-50 MB（取决于系统尺寸）
- **N 进程总内存**：约 20-50 MB × N（每个进程独立维护数据）
- 适合在多核工作站上运行

## 扩展方向

- 更多系统尺寸对比（例如 N = 50, 100, 200, 400, 800, 1600）
- 不同边界条件（开放边界、固定边界）
- 添加外磁场效应
- 自关联函数分析
- 二维 Ising 模型（有相变）
- 动态性质分析（自动关联时间）

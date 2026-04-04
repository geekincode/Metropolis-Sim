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

```bash
# 1. 编译
cd /home/geekincode/Physics/MC
mkdir -p build && cd build
cmake ..
make -j12

# 2. 后台运行模拟（推荐）
cd /home/geekincode/Physics/MC
nohup ./build/ising1d > log &

# 3. 监视进度
tail -f log

# 4. 计算完成后生成图表
gnuplot scripts/plot.gnu

# 5. 查看结果
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
- Gnuplot（用于绘图）

安装 gnuplot：
```bash
sudo apt-get install gnuplot  # Ubuntu/Debian
```

### 编译步骤

```bash
cd /home/geekincode/Physics/MC
mkdir build && cd build
cmake ..
make
```

### 运行模拟

**方式一：前台直接运行**

```bash
cd /home/geekincode/Physics/MC/build
./ising1d
```

模拟将运行约 5-10 分钟，数据会自动保存到项目根目录的 `data/` 文件夹。

**方式二：后台运行（推荐用于长时间计算）**

当计算时间较长时，使用 `nohup` 在后台执行：

```bash
cd /home/geekincode/Physics/MC
nohup ./build/ising1d > log &
```

**后台运行管理命令**

```bash
# 查看后台进程
jobs
ps aux | grep ising1d

# 实时查看输出日志
tail -f log

# 检查计算进度（查看已生成的数据文件）
ls -lh data/

# 停止后台进程
kill %1          # 根据 jobs 输出的编号
# 或
pkill -f ising1d
```

**说明**：
- `nohup` - 让进程在终端关闭后继续运行
- `> log` - 将标准输出重定向到 `log` 文件
- `&` - 放在后台执行
- `tail -f log` - 实时监视日志输出（按 Ctrl+C 退出）

### 生成图表

在模拟完成后（或使用 `tail -f log` 看到"所有模拟完成"提示），运行：

```bash
cd /home/geekincode/Physics/MC
gnuplot scripts/plot.gnu
```

图表将保存为 `plots/results.png`。

**如果使用后台运行**：

```bash
# 监视日志，等待计算完成
tail -f log

# 看到"所有模拟完成"后，按 Ctrl+C 退出 tail

# 生成图表
gnuplot scripts/plot.gnu

# 查看结果
open plots/results.png
```

## 模拟参数

| 参数 | 值 | 说明 |
|------|-----|------|
| **系统尺寸** | N = 100, 400, 900 | 多尺寸对比研究，观察有限尺寸效应 |
| **耦合常数** | J = 1.0 | Ising 模型耦合强度 |
| **温度范围** | T = 0.1 ~ 5.0 | 从低温到高温扫描 |
| **温度步长** | ΔT = 0.1 | 温度点数 = 49 |
| **平衡化扫描** | nEquil = 50000 | 热化阶段（数据不计入统计） |
| **测量扫描** | nMeasure = 100000 | 测量阶段（计入统计） |
| **测量间隔** | 每 10 次扫描 | 采样 10000 个数据点/温度点 |

### 参数调整

修改 `src/main.cpp` 第 13-22 行：

```cpp
std::vector<int> systemSizes = {100, 400, 900};  // 修改系统尺寸
const int nEquil = 50000;                        // 修改平衡化扫描数
const int nMeasure = 100000;                     // 修改测量扫描数
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

### Q1: 后台运行的进程如何确认还在运行？

```bash
# 方法1：查看进程
ps aux | grep ising1d

# 方法2：查看日志更新
tail -f log

# 方法3：检查数据文件大小是否在增长
watch -n 5 'ls -lh data/'
```

### Q2: 如何停止正在运行的后台进程？

```bash
# 优雅停止
kill %1          # 如果用 jobs 看到编号

# 或强制杀死
pkill -f ising1d
```

### Q3: `nohup` 和直接运行有什么区别？

| 方式 | 优点 | 缺点 |
|------|------|------|
| 直接运行 `./ising1d` | 实时看到输出 | 关闭终端进程会停止 |
| `nohup ./ising1d > log &` | 可关闭终端，进程继续运行 | 需要用 `tail -f log` 查看输出 |

### Q4: 计算结果数据在哪里？

所有数据文件都保存在 `data/` 目录下：
- `magnetization_N100.dat` - 磁化强度数据（N=100）
- `susceptibility_N100.dat` - 磁化率数据（N=100）
- `energy_N100.dat` - 能量数据（N=100）
- ...以此类推 N=400, 900

### Q5: 如何加快计算速度？

在 `src/main.cpp` 中减小参数：

```cpp
const int nEquil = 10000;      // 减小平衡化扫描（默认 50000）
const int nMeasure = 20000;    // 减小测量扫描（默认 100000）
const double T_step = 0.5;     // 增大温度步长（默认 0.1）
```

然后重新编译：`cd build && make -j12`

### Q6: Gnuplot 画图失败？

确保安装了 Gnuplot 并且 `scripts/plot.gnu` 文件存在：

```bash
# macOS
brew install gnuplot

# Ubuntu/Debian
sudo apt-get install gnuplot

# 检查安装
gnuplot --version
```

## 扩展方向

- 不同边界条件（开放边界）
- 添加外磁场
- 自关联函数分析
- 二维 Ising 模型（有相变）

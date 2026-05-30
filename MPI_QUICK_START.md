# MPI 快速参考卡

## 编译

```bash
cd /home/geekincode/Physics/MC
mkdir -p build && cd build
cmake ..
make -j12
```

## 运行模拟

### 快速选项

```bash
# 前台运行（看实时输出）
cd /home/geekincode/Physics/MC/build
mpirun -n 4 ./ising1d

# 后台运行（推荐）
cd /home/geekincode/Physics/MC
nohup mpirun -n 4 ./build/ising1d > log &
```

### 选择进程数

```bash
mpirun -n 1 ./build/ising1d    # 1 进程（串行）
mpirun -n 2 ./build/ising1d    # 2 进程（双核）
mpirun -n 4 ./build/ising1d    # 4 进程（推荐）
mpirun -n 8 ./build/ising1d    # 8 进程（8+ 核）
```

## 监控计算

```bash
# 查看进程
ps aux | grep mpirun

# 实时查看日志
tail -f log

# 检查数据文件
ls -lh data/

# 自动刷新文件大小
watch -n 5 'ls -lh data/'
```

## 停止计算

```bash
# 查看后台任务
jobs

# 停止第一个后台任务
kill %1

# 强制停止所有 mpirun
pkill -f mpirun
```

## 生成图表

```bash
cd /home/geekincode/Physics/MC
gnuplot scripts/plot.gnu
open plots/results.png
```

## 调整参数

编辑 `src/main.cpp` 第 31-40 行：

```cpp
std::vector<int> systemSizes = {100, 400, 900};  // 系统尺寸
const double J = 5.0;                            // 耦合常数
const int nEquil = 500000;                       // 平衡化扫描
const int nMeasure = 1000000;                    // 测量扫描
const int measureInterval = 2;                   // 采样间隔
const double T_min = 0.1, T_max = 5.0;          // 温度范围
const double T_step = 0.1;                       // 温度步长
```

重新编译：
```bash
cd build && make -j12
```

## 快速配置预设

### 快速测试（1-2 分钟）
```cpp
const int nEquil = 50000;
const int nMeasure = 100000;
const int measureInterval = 10;
```

### 标准精度（3-5 分钟，当前配置）
```cpp
const int nEquil = 500000;
const int nMeasure = 1000000;
const int measureInterval = 2;
```

### 高精度（15-20 分钟）
```cpp
const int nEquil = 1000000;
const int nMeasure = 5000000;
const int measureInterval = 1;
```

## 故障排除

### MPI 安装检查
```bash
# 检查 MPI 是否安装
which mpirun
mpirun --version

# 如未安装，安装 MPI：
# Ubuntu: sudo apt-get install libopenmpi-dev
# macOS: brew install open-mpi
# CentOS: sudo yum install openmpi-devel
```

### 编译错误
```bash
# 清除旧编译文件
cd build
rm -rf *
cmake ..
make -j12
```

### 运行出错

```bash
# 检查数据目录是否存在
ls -la data/

# 手动创建目录
mkdir -p data plots

# 检查可执行文件
ls -la build/ising1d
```

## 数据输出

所有结果保存在 `data/` 目录：

- `magnetization_N100.dat`, `_N400.dat`, `_N900.dat` - 磁化强度
- `susceptibility_N100.dat`, `_N400.dat`, `_N900.dat` - 磁化率
- `energy_N100.dat`, `_N400.dat`, `_N900.dat` - 能量

绘图后生成：`plots/results.png`

## 完整工作流

```bash
# 1. 编译
cd /home/geekincode/Physics/MC
mkdir -p build && cd build && cmake .. && make -j12

# 2. 后台运行 4 进程
cd ..
nohup mpirun -n 4 ./build/ising1d > log &

# 3. 监控进度（可选）
# tail -f log

# 4. 等待完成，然后生成图表
gnuplot scripts/plot.gnu

# 5. 查看结果
open plots/results.png
```

---
**更新时间**：2026-04-05  
**相关文档**：[README.md](README.md) | [CLAUDE.md](CLAUDE.md)

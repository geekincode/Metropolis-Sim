# Gnuplot 脚本：绘制一维 Ising 模型多尺寸对比结果

# 设置输出格式
set terminal pngcairo enhanced size 1400,600 font 'Arial,14'
set output 'plots/results.png'

# 设置多图布局（1行2列）
set multiplot layout 1,2

# ========== 图 (a)：磁化强度 m 随温度的变化 ==========
set xlabel 'k_BT/J' font 'Arial,16'
set ylabel 'Magnetization m' font 'Arial,16'
set title '(a) Magnetization vs Temperature' font 'Arial,18'
set grid
set key top right

# 设置坐标轴范围
set xrange [0:5.5]
set yrange [0:1.1]

# 绘制不同系统尺寸的数据
plot 'data/magnetization_N100.dat' using 1:2 with linespoints \
     pt 7 ps 0.4 lw 2 lc rgb '#0072BD' title 'N=100', \
     'data/magnetization_N400.dat' using 1:2 with linespoints \
     pt 5 ps 0.4 lw 2 lc rgb '#D95319' title 'N=400', \
     'data/magnetization_N900.dat' using 1:2 with linespoints \
     pt 9 ps 0.4 lw 2 lc rgb '#77AC30' title 'N=900'

# ========== 图 (b)：磁化率 χ 随温度的变化 ==========
set xlabel 'k_BT/J' font 'Arial,16'
set ylabel 'Magnetic Susceptibility χ' font 'Arial,16'
set title '(b) Susceptibility vs Temperature' font 'Arial,18'
set grid
set key top right

# 设置坐标轴范围
set xrange [0:5.5]
set yrange [0:*]

# 绘制不同系统尺寸的数据
plot 'data/susceptibility_N100.dat' using 1:2 with linespoints \
     pt 7 ps 0.4 lw 2 lc rgb '#0072BD' title 'N=100', \
     'data/susceptibility_N400.dat' using 1:2 with linespoints \
     pt 5 ps 0.4 lw 2 lc rgb '#D95319' title 'N=400', \
     'data/susceptibility_N900.dat' using 1:2 with linespoints \
     pt 9 ps 0.4 lw 2 lc rgb '#77AC30' title 'N=900'

unset multiplot

print "图表已生成：plots/results.png"
print "图 (a) 展示磁化强度随温度的变化"
print "图 (b) 展示磁化率随温度的变化，体现有限尺寸效应"
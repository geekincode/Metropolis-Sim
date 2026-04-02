cmake_minimum_required(VERSION 3.10)
project(IsingModel1D)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项（优化和警告）
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

# 源文件
set(SOURCES
    src/IsingModel1D.cpp
    src/Observables.cpp
    src/main.cpp
)

# 可执行文件
add_executable(ising1d ${SOURCES})

# 创建输出目录
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/plots)

# 安装目标（可选）
install(TARGETS ising1d DESTINATION bin)
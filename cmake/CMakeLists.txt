# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(tools VERSION 1.1 LANGUAGES C)

# 设置编译选项
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label -D _GNU_SOURCE")
# set(CMAKE_BUILD_TYPE "Debug")
set(TOOLS_SOURCE_DIR ../sources/)

# 检测编译工具链（GCC,MSVC,...）

# 设置要引用的头文件目录
# include_directories(../sources/include)

# 全局宏定义
add_definitions(-D TOOLS_EXPORT)

aux_source_directory(${TOOLS_SOURCE_DIR}av TOOLS_LIBAV)
add_library(av SHARED ${TOOLS_SOURCE_DIR})

# target_link_libraries(cpp_test PRIVATE cpp)


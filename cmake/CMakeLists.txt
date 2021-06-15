# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(tools VERSION 1.1 LANGUAGES C)

# 设置编译选项
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label -D _GNU_SOURCE")
# set(CMAKE_BUILD_TYPE "Debug")
set(LIBTOOLS_DIR ../sources/libtools)

# 检测编译工具链（GCC,MSVC,...）

# 设置要引用的头文件目录
include_directories(${LIBTOOLS_DIR})

# 全局宏定义
# add_definitions(-D TOOLS_EXPORT)


# libtools
aux_source_directory(${LIBTOOLS_DIR} LIBTOOLS_SOURCE)
add_executable(tools ${LIBTOOLS_SOURCE})
target_link_libraries(tools PRIVATE pthread)

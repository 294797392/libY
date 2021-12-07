# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(Y VERSION 1.1 LANGUAGES C)

# 设置编译选项
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label -D _GNU_SOURCE")
# set(CMAKE_BUILD_TYPE "Debug")
set(Y_DIR ../sources/libY)

add_definitions(-D Y_ENV_MINGW)

# 检测编译工具链（GCC,MSVC,...）

# 设置要引用的头文件目录
include_directories(${Y_DIR})
include_directories(${Y_DIR}/Ylog)

# 全局宏定义
# add_definitions(-D TOOLS_EXPORT)

# 编译libY
list(APPEND Y_SOURCE ${Y_DIR}/Ybase.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ythread.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ystring.c)
list(APPEND Y_SOURCE ${Y_DIR}/Yqueue.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ytree.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylist.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylog/Ylog.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylog/Yappender_file.c)
# aux_source_directory(${Y_DIR} Y_SOURCE)
add_library(Y ${Y_SOURCE})
target_link_libraries(Y PRIVATE pthread)

# 编译Yexe
add_executable(Yexe ${Y_DIR}/main.c)
target_link_libraries(Yexe PRIVATE Y)


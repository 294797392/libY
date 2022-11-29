# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(Y VERSION 1.1 LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label -Wno-unused-function -D _GNU_SOURCE -fPIC")

# 全局变量定义
set(Y_DIR ../sources/libY)
set(Y_EXT_DIR ../external)
set(Y_SYS ${CMAKE_SYSTEM_NAME})
message(STATUS "Y_SYS = " ${Y_SYS})

# 宏定义选项
add_definitions(-D Y_MAP_CHAIN)                 # 指定哈希表使用链式扩展法解决哈希冲突问题

# 配置不同环境下的CMake文件
if(${MSYS})
    message(STATUS "USE CMakeLists.msys.txt")
    include(CMakeLists.msys.txt)
elseif(${MINGW})
    message(STATUS "USE CMakeLists.mingw.txt")
    include(CMakeLists.mingw.txt)
elseif(${UNIX})
    message(STATUS "USE CMakeLists.unix.txt")
    include(CMakeLists.linux.txt)
else()
endif()

# 设置要引用的头文件目录
include_directories(${Y_DIR})
include_directories(${Y_DIR}/Ylog)
include_directories(${Y_EXT_DIR}/cJSON)

# 编译libY
list(APPEND Y_SOURCE ${Y_DIR}/Ystring.c)
list(APPEND Y_SOURCE ${Y_DIR}/Yqueue.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ybuffer_queue.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ytree.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylist.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ypool.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ymap.c)
list(APPEND Y_SOURCE ${Y_DIR}/Yfile.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ythread.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylog/Ylog.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylog/Yappender_conosle.c)
list(APPEND Y_SOURCE ${Y_EXT_DIR}/cJSON/cJSON.c)
add_library(Y ${Y_SOURCE})
target_link_libraries(Y PRIVATE ${Y_LINK_LIBRARIES})

# 编译Yexe
add_executable(Ydemo ${Y_DIR}/main.c)
target_link_libraries(Ydemo PRIVATE Y)

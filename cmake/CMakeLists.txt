# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(Y VERSION 1.1 LANGUAGES C)

# 全局变量定义
set(Y_DIR ../sources/libY)
add_definitions(-D Y_MAP_CHAIN)     # 指定哈希表使用链式扩展法解决哈希冲突问题


# 配置不同环境下的CMake文件
include(CMakeLists.mingw.txt)

# 设置要引用的头文件目录
include_directories(${Y_DIR})
include_directories(${Y_DIR}/Ylog)

# 编译libY
list(APPEND Y_SOURCE ${Y_DIR}/Ybase.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ythread.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ystring.c)
list(APPEND Y_SOURCE ${Y_DIR}/Yqueue.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ytree.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylist.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ypool.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ynet.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ymap.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylog/Ylog.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylog/Yappender_conosle.c)
list(APPEND Y_SOURCE ${Y_DIR}/Ylock.c)
# aux_source_directory(${Y_DIR} Y_SOURCE)
add_library(Y ${Y_SOURCE})
target_link_libraries(Y PRIVATE pthread ${Y_LINK_LIBRARIES})

# 编译Yexe
add_executable(Ydemo ${Y_DIR}/main.c)
target_link_libraries(Ydemo PRIVATE Y)

# 第三方开发者要使用的头文件
list(APPEND Y_INCLUDE ${Y_DIR}/Yfirstinclude.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Yerrno.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ybase.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ynet.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ypool.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Yqueue.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ystring.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ythread.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ytree.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ymap.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ylog/Ylog.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ylog/Ylogbase.h)
list(APPEND Y_INCLUDE ${Y_DIR}/Ylock.h)

# 拷贝头文件到libYinc
file(COPY ${Y_INCLUDE} DESTINATION ${Y_DIR}/../libYinc)

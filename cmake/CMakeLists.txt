# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(tools VERSION 1.1 LANGUAGES C)

# 设置编译选项
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label -D _GNU_SOURCE")
# set(CMAKE_BUILD_TYPE "Debug")
set(SOURCE_DIR ../sources/)
set(ACAP_DRIVER_DIR ${SOURCE_DIR}av/acap/drivers)

# 检测编译工具链（GCC,MSVC,...）

# 设置要引用的头文件目录
include_directories(${SOURCE_DIR})

# 全局宏定义
# add_definitions(-D TOOLS_EXPORT)


# libav
#aux_source_directory(${SOURCE_DIR}av LIBAV_SOURCE)
#list(APPEND LIBAV_SOURCE ${ACAP_DRIVER_DIR}/acap_mci.c)
#add_library(av SHARED ${LIBAV_SOURCE})
#target_link_libraries(av PRIVATE Winmm)

# acapdrv
aux_source_directory(${SOURCE_DIR}av ACAP_DRIVER_SOURCE)
list(APPEND ACAP_DRIVER_SOURCE ${ACAP_DRIVER_DIR}/acap_mci.c)
list(APPEND ACAP_DRIVER_SOURCE ${ACAP_DRIVER_DIR}/acap_driver_main.c)
add_executable(acapdrv ${ACAP_DRIVER_SOURCE})
target_link_libraries(acapdrv PRIVATE Winmm)
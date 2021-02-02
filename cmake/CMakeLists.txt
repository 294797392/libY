# vim: ff=unix ts=4 sw=4 et ai

cmake_minimum_required(VERSION 3.4)
project(devtools VERSION 1.1 LANGUAGES C)

# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-label -D _GNU_SOURCE")
set(CMAKE_BUILD_TYPE "Debug")
SET(DEVTOOLS_SRC_BASE_DIR ../sources/)

# 检测编译工具链（GCC,MSVC,...）
if(NOT DEFINED DEVTOOLS_TOOLCHAIN)
    set(DEVTOOLS_TOOLCHAIN GCC)
else()
    set(DEVTOOLS_TOOLCHAIN ${DEVTOOLS_TOOLCHAIN})
endif()

# 设置要引用的头文件目录
# include_directories(../sources/include)

# 全局宏定义
add_definitions(-D DEVTOOLS_EXPORT)
add_definitions(-D ${DEVTOOLS_TOOLCHAIN})

# aux_source_directory(../sources APP_GUID_SRC_FILES)
LIST(APPEND APP_GUID_SRC_FILES ${DEVTOOLS_SRC_BASE_DIR}guid.c)
add_executable(guid ${APP_GUID_SRC_FILES})


# add_library(yxf SHARED ${LIBYXF_SRC_FILES})

# target_link_libraries(cpp_test PRIVATE cpp)

message(STATUS "DEVTOOLS_TOOLCHAIN = ${DEVTOOLS_TOOLCHAIN}")
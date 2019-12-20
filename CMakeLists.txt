cmake_minimum_required(VERSION 3.15)
project(ssbl C)

set(CMAKE_C_STANDARD 11)

include_directories(/home/raifer/gap_sdk/rtos/pmsis/pmsis_bsp/include)

add_executable(ssbl
        ssbl.c traces.h)

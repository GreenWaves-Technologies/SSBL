cmake_minimum_required(VERSION 3.15)
project(ssbl C)

set(CMAKE_C_STANDARD 11)

include_directories(/g/bsp/include)
include_directories(/g/api/include)
include_directories(/g/driver)

add_definitions(PMSIS_DRIVERS)

add_executable(ssbl
        ssbl.c
        partition.c partition.h
        bootloader_utility.c bootloader_utility.h
        )

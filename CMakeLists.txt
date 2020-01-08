cmake_minimum_required(VERSION 3.15)
project(ssbl C)

set(CMAKE_C_STANDARD 11)

include_directories(/home/raifer/gap_sdk/rtos/pmsis/pmsis_bsp/include)
include_directories(/home/raifer/gap_sdk/rtos/pmsis/pmsis_api/include)
include_directories(/home/raifer/gap_sdk/rtos/pmsis/pmsis_driver)

add_definitions(PMSIS_DRIVERS)

add_executable(ssbl
        ssbl.c
        traces.h
        partition.c partition.h
        pi_partition.c pi_partition.h
        pi_errno.h
        pi_flash_partition.h pi_flash_partition.c
        bootloader_utility.c bootloader_utility.h
        md5.h md5.c
        )

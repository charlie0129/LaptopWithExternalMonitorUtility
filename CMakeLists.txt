cmake_minimum_required(VERSION 3.15)
project(LaptopWithExternalMonitorUtility)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-O3")

include_directories(.)

add_executable(LaptopWithExternalMonitorUtility
        LaptopWithExternalMonitorUtility.cpp)
cmake_minimum_required(VERSION 3.3.2)
project(sample CXX)
find_package(HPX REQUIRED)
add_hpx_executable(matrix_mul
    SOURCES matrix_mul.cpp
    COMPONENT_DEPENDENCIES iostreams)
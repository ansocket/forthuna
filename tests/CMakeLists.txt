cmake_minimum_required(VERSION 3.31)

project(tests C CXX)

add_compile_options(-g3 )
add_library(forth ../forth/forth.c ../forth/forth_core.c)
target_include_directories(forth PUBLIC ../forth)

add_executable(core core/core.cpp)
target_link_libraries(core PRIVATE gtest gtest_main forth)
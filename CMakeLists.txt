cmake_minimum_required(VERSION 3.31)

project(forthuna C CXX)

add_library(forthuna forth/forth.c forth/forth_core.c)
target_include_directories(forthuna PUBLIC forth)

if(EXAMPLE_REPL)
    add_subdirectory(examples/repl)
endif()
cmake_minimum_required(VERSION 3.31)

project(forthuna C CXX)

if(CMAKE_BUILD_TYPE EQUAL "Debug")
    add_compile_options(-O0 -g3 -Wall)
elif(CMAKE_BUILD_TYPE EQUAL "Release")
    add_compile_options(-Os -Wall)
endif()

add_subdirectory(forth)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
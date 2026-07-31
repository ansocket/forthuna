#!/bin/bash

cmake -S .. -B ../build_tests -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug && 
cmake --build ../build_tests
#!/bin/bash

cmake -S .. -B ../build_tests -DBUILD_TESTS=ON -DSETS_CUSTOM_SET=ON -DCMAKE_BUILD_TYPE=Debug && 
cmake --build ../build_tests && 
../build_tests/tests/core/core
../build_tests/tests/custom_set/custom_set_tests
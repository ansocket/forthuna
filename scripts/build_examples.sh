#!/bin/bash

cmake -S .. -B ../build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug &&
cmake --build ../build
#!/bin/bash

cmake -S .. -B ../build -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug &&
cmake --build ../build
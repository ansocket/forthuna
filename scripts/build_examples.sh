#!/bin/bash

cmake -S .. -B ../build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DSETS_CUSTOM_SET=ON -DCMAKE_BUILD_TYPE=Debug &&
cmake --build ../build
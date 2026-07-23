#!/bin/bash

build/core &&
lcov --capture --directory build/CMakeFiles/forth.dir --output-file build/coverage.info --ignore-errors mismatch &&
lcov --summary build/coverage.info | grep "lines......:"

#!/bin/bash

build/core &&
lcov --capture --directory . --output-file build/coverage.info --ignore-errors mismatch &&
lcov --summary build/coverage.info | grep "lines......:"
#!/bin/bash

../build_tests/tests/core/core &&
lcov --capture --directory ../build_tests/forth --output-file ../build_tests/coverage.info --ignore-errors mismatch &&
lcov --summary ../build_tests/coverage.info | grep "lines......:"

#!/bin/bash

test_dir=test
type=Release

cd $test_dir

if [ ! -d build/$type ]; then
	cmake -B build/$type -DCMAKE_BUILD_TYPE=$type
fi

cmake --build build/$type --target clean
cmake --build build/$type

ctest --test-dir build/$type -V

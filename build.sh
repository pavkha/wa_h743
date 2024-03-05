#!/bin/bash

type=$1

case "$type" in
"" | "debug")
	type=Debug
	;;

"release")
	type=Release
	;;

*)
	echo "usage: ./build [debug] | release"
	exit
	;;
esac

echo "build $type"

if [ ! -d build/$type ]; then
	cmake -B build/$type -DCMAKE_BUILD_TYPE=$type
fi

cmake --build build/$type --target clean
cmake --build build/$type

if [[ $# == 2 && $2 == "test" ]]; then
	./test.sh
fi

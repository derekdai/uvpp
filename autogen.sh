#!/bin/bash
src_dir=$(dirname $(readlink -f $0))

if [[ "$PWD" != "$src_dir" ]]; then
    for f in *; do
        rm -rf "$f"
    done
fi

CXXFLAGS=-fno-rtti \
cmake "$src_dir" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_VERBOSE_MAKEFILE=True \
    -G"Eclipse CDT4 - Unix Makefiles"

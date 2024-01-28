#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
pushd $SCRIPT_DIR > /dev/null
pushd .. > /dev/null

mkdir -p build

BUILD_ARGS=(
    ./src/*.c
    -o ./build/daylog
    -g # enable debugging
    -Wall -Wextra -Wpedantic
    -Wformat=2 -Wno-unused-parameter -Wshadow
    -Wwrite-strings -Wstrict-prototypes -Wold-style-definition
    -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
    -Wjump-misses-init -Wlogical-op
)

gcc "${BUILD_ARGS[@]}"
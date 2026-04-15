#!/bin/bash

if [ ! -d "./build" ]; then
    mkdir ./build
else
    rm -rf ./build
    mkdir ./build
fi

cd ./build || exit 1
cmake ../
# shellcheck disable=SC2154
make -j"${nproc}"
printf "demo running...\n"
./demo

#!/bin/bash

build() {
    gcc -o murray src/main.c -lraylib -lm
}

if [ "$1" == "build" ]; then
    build
elif [ "$1" == "run" ]; then
    build
    nvidia-offload ./murray
else
    echo invalid option $1
fi

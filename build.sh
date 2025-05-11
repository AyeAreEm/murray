#!/bin/bash

if [ "$1" == "build" ]; then
    gcc -o murray main.c -lraylib -lm
elif [ "$1" == "run" ]; then
    gcc -o murray main.c -lraylib -lm
    nvidia-offload ./murray
else
    echo invalid option $1
fi

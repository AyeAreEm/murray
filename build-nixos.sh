#!/usr/bin/env bash

build() {
    gcc -o murray src/main.c src/menu.c src/game.c src/gameover.c src/player.c src/zombie.c src/bullet.c -lraylib -lm
}

if [ "$1" == "build" ]; then
    build
elif [ "$1" == "run" ]; then
    build
    nvidia-offload ./murray
else
    echo invalid option $1
fi

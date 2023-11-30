#!/bin/sh
mkdir -p build
RAYLIB_PATH=external/raylib-5.0/src/
WORKING_DIR=$(pwd)
(cd $RAYLIB_PATH && make PLATFORM=PLATFORM_DESKTOP RAYLIB_RELEASE_PATH=$WORKING_DIR/build)
gcc -Wall -Wextra -g -o zoomer zoomer.c -I $RAYLIB_PATH -L ./build/ -lraylib -lm

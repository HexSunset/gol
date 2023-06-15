#!/bin/bash
set -xe

LIBS="-lraylib -lm"
OPTS="-Wall"
SRCS="./src/gol.c"

gcc $OPTS -o ./build/gol $LIBS $SRCS && ./build/gol file noise.png

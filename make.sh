#!/bin/bash

LIBS="-lraylib -lm"
OPTS="-Wall -g"
SRCS="./src/gol.c"

set -xe

gcc $OPTS -o ./build/gol $LIBS $SRCS

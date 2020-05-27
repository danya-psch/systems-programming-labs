#!/bin/sh
cc -Wall -Werror -g -ggdb server.c -o server -lrt
./server

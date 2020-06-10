#!/bin/sh
cc -Wall -Werror -g -ggdb src/* -o a.out
./a.out -t transmit.txt

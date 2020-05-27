#!/bin/sh
cc -Wall -Werror -g -ggdb client.c -o client -lrt
./client

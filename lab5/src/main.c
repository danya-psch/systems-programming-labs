#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/layer4.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Not enough arguments\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-t") == 0) {
        layer4_transmit(argv[2]);
    } else if (strcmp(argv[1], "-r") == 0) {
        layer4_receive(argv[2]);
    } else {
        fprintf(stderr, "Unknown mode");
        return EXIT_FAILURE;
    }

    return 0;
}
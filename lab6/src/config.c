#include <config.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

config_t get_config_from_args(int argc, char * argv[]) {
    int opt;
    config_t config = DEFAULT_CONFIG;

    while ((opt = getopt(argc, argv, "p:d:t:q:l:")) != -1) {
        switch (opt) {
            case 'p': {
                config.port = strtoul(optarg, NULL, 10);
                if (config.port < 0 || config.port > 65535) {
                    fprintf(stderr, "Invalid port, should be > 0 and < 65535\n");
                    exit(EXIT_FAILURE);
                }
            } break;
            case 'd': {
                if (strcmp(optarg, "") == 0) {
                    fprintf(stderr, "Root server path couldn't be empty\n");
                    exit(EXIT_FAILURE);
                }
                memcpy(config.root_path, optarg, strlen(optarg));
                config.root_path[strlen(optarg)] = 0;
            } break;
            case 't': {
                unsigned long num_threads = strtoul(optarg, NULL, 10);
                if (num_threads < 0 || num_threads > 16) {
                    fprintf(stderr, "Invalid number of threads, should be > 0 and < 16\n");
                    exit(EXIT_FAILURE);
                }
                config.num_threads = (int) num_threads;
            } break;
            case 'q': {
                unsigned long queue_length = strtoul(optarg, NULL, 10);
                if (queue_length < 0 || queue_length > 128) {
                    fprintf(stderr, "Invalid length of queue, should be > 0 and < 128\n");
                    exit(EXIT_FAILURE);
                }
                config.queue_length = (int) queue_length;
            } break;
            case 'l': {
                if (strcmp(optarg, "") == 0) {
                    fprintf(stderr, "Log file path couldn't be empty\n");
                    exit(EXIT_FAILURE);
                }
                memcpy(config.log_path, optarg, strlen(optarg));
                config.log_path[strlen(optarg)] = 0;
            } break;
            default: {
                fprintf(stderr, "Usage: %s [-p port] [-d root path] [-t num threads] "
                                "[-q queue length] [-l log file]\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }

    return config;
}

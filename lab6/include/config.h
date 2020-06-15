#pragma once

#include <stdint.h>
#include <linux/limits.h>

#define DEFAULT_CONFIG              \
    {                               \
        .log_path = "server.log",   \
        .queue_length = 15,         \
        .num_threads = 4,           \
        .root_path = "..",          \
        .port = 8080                \
    }

typedef struct {
    uint16_t port;
    char root_path[PATH_MAX];
    int num_threads;
    int queue_length;
    char log_path[PATH_MAX];
} config_t;

config_t get_config_from_args(int argc, char * argv[]);

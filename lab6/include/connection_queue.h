#pragma once

#include <connection.h>
#include <pthread.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <config.h>


typedef struct {
    connection_t *start;
    connection_t *end;
    pthread_mutex_t lock;
    sem_t is_empty_queue;
    config_t *config;
    int fd_log;
} connection_queue_t;

connection_queue_t * connection_queue_init(void);
int connection_queue_push(connection_queue_t * self, connection_t * conn);
connection_t * connection_queue_pull(connection_queue_t * self);
void connection_queue_destroy(connection_queue_t * self);

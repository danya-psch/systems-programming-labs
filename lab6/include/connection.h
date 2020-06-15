#pragma once

#include <http_request.h>
#include <netinet/in.h>

typedef struct connection {
    struct connection *next;
    int client_fd;
    struct sockaddr_in *client_address;
    http_request_t *request;
} connection_t;

connection_t * connection_new();
void connection_destroy(connection_t *self);

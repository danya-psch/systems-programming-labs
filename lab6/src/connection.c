#include <connection.h>
#include <stdlib.h>

connection_t * connection_new() {
    connection_t * result = malloc(sizeof(connection_t));
    result->request = NULL;
    result->client_address = NULL;
    result->client_fd = -1;
    result->next = NULL;
    return result;
}

void connection_destroy(connection_t * self) {
    free(self);
}

#include <connection_queue.h>
#include <thread_pool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <handle.h>
#include <config.h>

#define BACKLOG 5

#define CLIENT_REQ_BUFF_SIZE 2048

#define ERROR(str, arg)                                                                         \
    if (arg < 0) {                                                                              \
        perror(str);                                                                            \
        exit(errno);                                                                            \
    } 

#define ERROR_OBJ(str, obj)                                                                     \
    if (obj == NULL) {                                                                          \
        fprintf(stderr, str);                                                                   \
        exit(errno);                                                                            \
    } 

connection_t * handle_request(int client_socket_fd, struct sockaddr_in * client_address);

connection_queue_t *conn_queue;
thread_pool_t *thread_pool;

bool shutdown_server = false;

void shutdown_handler(int signo) {
    shutdown_server = true;
}

void * handle(void * data);

pid_t daemonize() {
    pid_t child = fork();

    if (child != 0) {
        printf("Daemonized: %i\n", child);
        exit(EXIT_SUCCESS);
    } 

    setsid();

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    dup(STDOUT_FILENO);
    return child;
}

int main(int argc, char * argv[]) {
    config_t config = get_config_from_args(argc, argv);
    
    daemonize();

    pid_t pid = getpid();
    int fd_log;

    char cwd[MY_PATH_LEN];
    strncat(cwd, config.log_path, strlen(config.log_path));
    fd_log = open(cwd, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    ERROR("open: ", fd_log);

    dprintf(fd_log, "Server created [PID %d]\n", pid);

    conn_queue = connection_queue_init();

    conn_queue->fd_log = fd_log;
    conn_queue->config = &config;

    thread_pool = thread_pool_init(config.num_threads, conn_queue, handle);

    struct sockaddr_in adrr = {
        .sin_family = PF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(config.port),
    };

    int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    ERROR("socket: ", server_socket_fd);
    dprintf(fd_log, "Socket created\n");

    ERROR("bind: ", bind(server_socket_fd, (struct sockaddr *) &adrr, sizeof(struct sockaddr_in)));
    dprintf(fd_log, "Socket bind at port: %d\n", config.port);

    ERROR("listen: ", listen(server_socket_fd, BACKLOG));
    dprintf(fd_log, "Starting listening for requests...\n");

    signal(SIGPIPE, shutdown_handler);
    signal(SIGTERM, shutdown_handler);

    while (true) {
        int client_socket_fd = accept(server_socket_fd, NULL, NULL);
        ERROR("accept: ", client_socket_fd);
        dprintf(fd_log, "Accepted connection from client\n");

        connection_t *cur_conn = handle_request(client_socket_fd, &adrr);
        ERROR_OBJ("handle_request", cur_conn);

        ERROR("connection_queue_push: ", connection_queue_push(conn_queue, cur_conn));
    }
}

connection_t * handle_request(int client_socket_fd, struct sockaddr_in * client_address) {
    size_t len = CLIENT_REQ_BUFF_SIZE;
    char buff[len + 1];
    len = read_from_client(client_socket_fd, buff, len);
    if (len < 0) {
        return NULL;
    }
    buff[len] = 0;
    http_request_t * request = parse_request(buff);
    if (request == NULL) {
        return NULL;
    }
    connection_t * con = connection_new();
    if (con == NULL) {
        return NULL;
    }
    con->request = request;
    con->client_fd = client_socket_fd;
    con->client_address = client_address;
    return con;
}
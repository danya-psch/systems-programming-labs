#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFF_SIZE 512
#define MESSAGE_BUFF_SIZE 1024
#define PORT 3215 //variant 15
#define LISTEN_BACKLOG 50 

#define STR_WITH_SIZE(str) str "\n", strlen(str) + 1

#define PRINT_TO_LOG_ARGS(buff, str, ...)                                                       \
    if (log_file != 0) {                                                                        \
        snprintf(buff, sizeof(buff), str, __VA_ARGS__);                                         \
        write(log_file, strcat(buff, "\n"), strlen(buff) + 1);                                  \
        memset(buff, '\0', sizeof(char) * BUFF_SIZE );                                          \
    }                                                                                           \

#define RESPONCE_ACTION(buff, pid, msg)                                                         \
    time_t rawtime;                                                                             \
    time ( &rawtime );                                                                          \
    struct tm * timeinfo = localtime ( &rawtime );                                              \
    char* str_time = asctime (timeinfo);                                                        \
    str_time[strlen(str_time)-1] = 0;                                                           \
    snprintf(buff, sizeof(buff), "(pid:%u, time:%s) %s", pid, str_time, msg);                   \

#define PRINT_TO_LOG(str)                                                                       \
    if (log_file != 0) {                                                                        \
        write(log_file, str "\n", strlen(str) + 1);                                             \
    }                                                                                           \

#define ERROR(str, arg)                                                                         \
    if (arg < 0) {                                                                              \
        perror(str);                                                                            \
        exit(errno);                                                                            \
    } 


void response_actions(char* buff, char* msg);

int log_file;

int main() {
    pid_t fork_pid = fork();
    if (fork_pid < 0){                                            
        perror("Can not fork");
    } else if (fork_pid == 0) {
        pid_t pid = getpid();
        ERROR("setsid: ", setsid())
        // chdir("/");

        char log_buff[BUFF_SIZE];
        log_file = open("./log.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
        ERROR("open: ", log_file)

        PRINT_TO_LOG_ARGS(log_buff, "Server started [PID %d]", pid);

        struct sockaddr_in server_adrr = {
            .sin_family = PF_INET,
            .sin_addr.s_addr = htonl(INADDR_ANY),
            .sin_port = htons(PORT),
        };
        int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
        ERROR("socket: ", server_socket_fd)
        PRINT_TO_LOG("Socket created")

        ERROR("bind: ", bind(server_socket_fd, (struct sockaddr *)&server_adrr, sizeof(struct sockaddr_in)))
        PRINT_TO_LOG_ARGS(log_buff, "Socket binded at port: %d", PORT)

        ERROR("listen: ", listen(server_socket_fd, LISTEN_BACKLOG))
        PRINT_TO_LOG("Starting listening for requests");

        while(true) {
            int client_scoket_fd = accept(server_socket_fd, NULL, NULL);
            ERROR("Can not accept client scoket: ", client_scoket_fd)
            PRINT_TO_LOG("Accepted connection from client");

            pid_t subprocess_pid = fork();
            if (subprocess_pid < 0) {
                ERROR("Couldn't fork for new request: %s\n", strerror(errno));
                exit(errno);
            } else if (subprocess_pid == 0) {
                pid_t handler_pid = getpid();
                PRINT_TO_LOG_ARGS(log_buff, "Started subprocess to handle request with PID: %d", handler_pid);

                while(true) {
                    char msg_buff[MESSAGE_BUFF_SIZE];
                    ssize_t received_bytes = recv(client_scoket_fd, msg_buff, MESSAGE_BUFF_SIZE - 1, 0);
                    ERROR("recv: ", received_bytes)

                    PRINT_TO_LOG_ARGS(log_buff, "Subprocess (PID: %d) received message: %s", handler_pid, msg_buff);

                    char res_buff[MESSAGE_BUFF_SIZE];
                    RESPONCE_ACTION(res_buff, handler_pid, msg_buff)

                    ERROR("send: ", send(client_scoket_fd, res_buff, strlen(res_buff), 0))
                    PRINT_TO_LOG_ARGS(log_buff, "Subprocess (PID: %d) send message: `%s`", handler_pid, res_buff);

                    if (strncmp(msg_buff, "close", received_bytes) == 0) break;
                } 

                close(client_scoket_fd);
                PRINT_TO_LOG_ARGS(log_buff, "Subprocess (PID: %d) closed client socket", handler_pid);
                exit(EXIT_SUCCESS);
            } else {
                PRINT_TO_LOG_ARGS(log_buff, "Forked subprocess to handle request with PID: %d", subprocess_pid);
            }
        }
    } else {
        exit(EXIT_SUCCESS);
    }
}
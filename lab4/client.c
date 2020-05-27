#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MESSAGE_BUFF_SIZE 1024
#define PORT 3215 //variant 15    

#define ERROR(str, arg)                                                                         \
    if (arg < 0) {                                                                              \
        perror(str);                                                                            \
        exit(errno);                                                                            \
    } 

int main() {
    struct sockaddr_in client_adrr = {
        .sin_family = PF_INET,
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
        .sin_port = htons(PORT),
    };

    int client_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    ERROR("socket: ", client_socket_fd)
    printf("Socket created\n");

    ERROR("connect: ", connect(client_socket_fd, (struct sockaddr *)&client_adrr, sizeof(struct sockaddr_in)))
    printf("Connected to server socket\n");

    while (true) {
        printf("Enter msg: ");
        char msg_buff[MESSAGE_BUFF_SIZE];
        if (fgets(msg_buff, MESSAGE_BUFF_SIZE, stdin) == NULL) {
            fprintf(stderr, "Couldn't connect to server socket: %s\n", strerror(errno));
            exit(errno);
        }
        size_t len = strlen(msg_buff);
        if (len > 0 && msg_buff[len - 1] == '\n') {
            msg_buff[len - 1] = 0;
        } else {
            msg_buff[len] = 0;
        }

        size_t msg_len = send(client_socket_fd, msg_buff, strlen(msg_buff), 0);
        ERROR("send: ", msg_len)

        char res_buff[MESSAGE_BUFF_SIZE];
        ssize_t received_bytes = recv(client_socket_fd, res_buff, MESSAGE_BUFF_SIZE - 1, 0);
        ERROR("recv: ", received_bytes)

        res_buff[received_bytes] = 0;
        if (received_bytes > 0 && res_buff[received_bytes - 1] == '\n') {
            received_bytes[res_buff - 1] = 0;
        }

        printf("%s\n", res_buff);
        if (strncmp(msg_buff, "close", strlen(msg_buff)) == 0) {
            printf("Got close event.\n");
            exit(EXIT_SUCCESS);
        }
    }
}
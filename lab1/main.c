#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h> 
#include <errno.h>
#include <poll.h>

#define BUFSIZE 512

#define ERROR(str, arg)                             \
    if (arg < 0) {                                  \
        perror(str);                                \
    }                                               \

void task1(char* fin_name, char* fout_name);
void task2_1();
void task2_2();

int main(int argc, char** argv) {
    if (argc < 2) return EXIT_FAILURE;
    switch (atoi(argv[1])) {
        case 1: {
            if (argc != 4) return EXIT_FAILURE;
            task1(argv[2], argv[3]);
            break;
        } 
        case 2: {
            task2_1();
            break;
        }
        case 3: {
            task2_2();
            break;
        }
        default : {
            fprintf(stderr, "invalid argv");
            return EXIT_FAILURE;
        }
    }
}


void task1(char* fin_name, char* fout_name) {
    int fin = open(fin_name, O_RDONLY);  
    ERROR("trouble with open fin: ", fin);
    int fout = open(fout_name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    ERROR("trouble with open fout: ", fout);

    char buffer[BUFSIZE];
    memset(buffer, 0, BUFSIZE);
    
    int size = 0;
    int count = 0;
    while (size = read(fin, buffer, BUFSIZE), size > 0) {
        for (int i = 0; i < BUFSIZE; ++i) {
            if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] -= 32;
                ++count;
            }
        }
        ERROR("Something wrong with writting: ", write(fout, buffer, size));
    }

    close(fin);

    fsync(fout);
    close(fout);

    printf("Amount of replacements: %i\n", count);

}

void task2_1(){
    int fd = 0;
    fd_set fd_in;
    struct timeval tv;

    char buffer[BUFSIZE];
    memset(buffer, 0, BUFSIZE);

    do {
        FD_ZERO(&fd_in);
        FD_SET(fd, &fd_in);

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int sret = select(fd + 1, &fd_in, NULL, NULL, &tv);

        if (sret == -1) {
            perror("select: ");
        } else if (sret == 0) {
            fprintf(stderr, "\ntimeout\n");
        } else {
            if (FD_ISSET( fd, &fd_in )) {
                memset(buffer, 0, BUFSIZE);
                int ret = read(fd, buffer, BUFSIZE - 1);
                printf("ret = %d\n", ret);

                if (ret > 0) {
                    printf("buf = %s\n", buffer);
                }
            }
        }
        
    } while(1);
}

void task2_2() {
    
    int fd = 0;  
    struct pollfd fds;

	fds.fd = fd; //STDIN_FILENO;
	fds.events = POLLIN;

    int timeout = 5000;

    char buffer[BUFSIZE];
    memset(buffer, 0, BUFSIZE);

    do {
	    int pret = poll(&fds, 1, timeout);

        if (pret == -1) {
            perror("poll: ");
        } else if (pret == 0) {
            fprintf(stderr, "\ntimeout\n");
        } else {
            if (fds.revents & POLLIN) {
                fds.revents = 0;

                memset(buffer, 0, BUFSIZE);
                int ret = read(fd, buffer, BUFSIZE - 1);
                printf("ret = %d\n", ret);

                if (ret > 0) {
                    printf("buf = %s\n", buffer);
                }
            }
        }
    } while(1);
}

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT_PARAMS(str) printf("%s: pid = %i, ppid = %i, gid = %i, sid = %i\n", str, getpid(), getppid(), getgid(), getsid(0))

#define STR_WITH_SIZE(str)  str "\n", strlen(str) + 1

#define ERROR(str, arg)                             \
    if (arg < 0) {                                  \
        perror(str);                                \
    }                                               \

void task1();
void task2();

void print_params_n_times(char* str, int n);
void print_log();

int main() {
    task1();
    task2();
}

void task1() {
    puts("task1");
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork: ");
    } else if (pid == 0) {
        puts("Here is a child process");
        print_params_n_times("Child", 20);
        exit(EXIT_SUCCESS);
    } else {
        printf("Child process with pid = %i was created\n", pid);
        print_params_n_times("Parent", 20);
        pid_t res = wait(NULL);
        if (res == -1) {
            perror("wait: ");
        } else if (pid == res) {
            puts("done");
        } else {
            perror("Some error: ");
        }
    }
}

void print_params_n_times(char* str, int n) {
    for (int i = 0; i < n; ++i) {
        PRINT_PARAMS(str);
    }
}

void task2() {
    puts("\ntask2");
    int log = open("plog.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    ERROR("plog: ", log);

    write(log, STR_WITH_SIZE("Start"));

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork: ");
    } else if (pid == 0) {
        pid_t nsid = setsid();
        if (nsid < 0) {
            perror("setsid: ");
        }

        chdir("/");
        close(log);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        open("/dev/null", O_RDONLY);
        open("/dev/null", O_WRONLY); 
        open("/dev/null", O_WRONLY); 

        log = open("/home/danya/clog.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
        ERROR("clog: ", log);
        
        print_log(log);

        while (1) {
            sleep(1);
        }

    } else {
        printf("Child process with pid = %i was created\n", pid);
        write(log, STR_WITH_SIZE("Child process was created"));
        exit(EXIT_SUCCESS);
    }
}

void print_log(int log) {
    char buffer[1000]; 
    sprintf(buffer, "Info: pid = %i, ppid = %i, gid = %i, sid = %i\n", getpid(), getppid(), getgid(), getsid(0));
    write(log, &buffer, strlen(buffer));
}
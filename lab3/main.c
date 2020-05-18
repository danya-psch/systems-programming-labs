#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h> 


#define STR_WITH_SIZE(str) str "\n", strlen(str) + 1

#define PRINT_TO_LOG_ARGS(buff, str, ...)                       \
    snprintf(buff, sizeof(buff), str, __VA_ARGS__);             \
    write(log_file, strcat(buff, "\n"), strlen(buff) + 1);      \

#define ERROR(str, arg)                                         \
    if (arg < 0) {                                              \
        perror(str);                                            \
    } 

#define SHM_ERROR(str, arg, shm_name)                           \
    if (arg < 0) {                                              \
        shm_unlink(shm_name);                                   \
        perror(str);                                            \
    } 

#define BUFF_SIZE 512

struct datum {
    pid_t pid;
    struct tm time_stamp;
    char str[256];
};
typedef struct datum datum_t;
   
void signal_handler( int signo, siginfo_t *si, void * ucontext );

void task1();
void task2();

int log_file;

int main(int argc, char** argv) {
    if (argc < 2) return EXIT_FAILURE;
    if (strcmp(argv[1], "task1") == 0) {
        task1();
    } else if (strcmp(argv[1], "task2") == 0) {
        task2();
    }
    return EXIT_SUCCESS;
}

void task1() {
    log_file = open("log.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644); 
    char buffer[BUFF_SIZE];
    PRINT_TO_LOG_ARGS(buffer, "Start with pid: %i", getpid());

    struct sigaction sa = {
        .sa_flags = SA_SIGINFO,
        .sa_sigaction = signal_handler
    };
    sigemptyset(&sa.sa_mask);

    struct sigaction old_sa;

    ERROR("sigaction: ", sigaction(SIGHUP, &sa, &old_sa));

    while (1) {
        sleep(5);
        write(log_file, STR_WITH_SIZE("....."));
    }
}


void signal_handler( int signo, siginfo_t *si, void * ucontext ) {                           
    char buffer[BUFF_SIZE];  
    write(log_file, STR_WITH_SIZE("Receive signal"));
    PRINT_TO_LOG_ARGS(buffer, "Signal number being delivered: %i", si->si_signo);
    PRINT_TO_LOG_ARGS(buffer, "Signal code: %i", si->si_code);
    PRINT_TO_LOG_ARGS(buffer, "Errno value associated with this signal: %i", si->si_errno);
    PRINT_TO_LOG_ARGS(buffer, "Process ID of sending process: %i", si->si_pid);
    PRINT_TO_LOG_ARGS(buffer, "Real user ID of sending process: %i", si->si_uid);
    PRINT_TO_LOG_ARGS(buffer, "Address at which fault occurred: %p", si->si_addr);
    PRINT_TO_LOG_ARGS(buffer, "Exit value or signal for process termination: %i", si->si_status);
    PRINT_TO_LOG_ARGS(buffer, "Band event for SIGPOLL/SIGIO: %ld", si->si_band);
}


void task2() {
    const char* shm_name = "file";
    int shm_d = shm_open(shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ERROR("shm_open", shm_d);

    SHM_ERROR("ftruncate", ftruncate(shm_d, sizeof(datum_t)), shm_name);

    datum_t *datum = mmap(NULL, sizeof(datum_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_d, 0);
    if (datum == MAP_FAILED) {
        shm_unlink(shm_name);
        perror("mmap");
    } 
    
    char str[BUFF_SIZE];
    time_t rawtime;
    while (1) {
        printf("Wait for input:\n");
        fgets(str, BUFF_SIZE, stdin);

        SHM_ERROR("", msync(datum, sizeof(datum_t), MS_SYNC), shm_name);
        printf("pid: %d\n", datum->pid);
        printf("time: %s", asctime(&(datum->time_stamp)));
        printf("str: %s\n", datum->str);

        datum->pid = getpid();
        // time(&datum->time_stamp);
        time(&rawtime);
        datum->time_stamp = (*localtime(&rawtime));
        strncpy(datum->str, str, BUFF_SIZE);
    }

}
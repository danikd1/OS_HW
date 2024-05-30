#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int fd[2];
int semid;

void cleanup() {
    close(fd[0]);
    close(fd[1]);
    semctl(semid, 0, IPC_RMID, NULL);
    printf("Resources cleaned up\n");
}

void signal_handler(int sig) {
    cleanup();
    exit(0);
}

void child(int* pd, int semid) {
    while(1)
    {
        char result[15] = "\0";
        read(pd[0], result, 15);
        printf("CHILD: %s\n", result);

        struct sembuf child_buf =
                {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
        if(semop(semid, &child_buf, 1) < 0){
            printf("Can\'t add 1 to semaphor\n");
            exit(-1);
        }
        write(pd[1], "Child to parent", 15);
        sleep(1);
    }
}

void parent(int* pd, int semid) {
    while(1)
    {
        write(pd[1], "Parent to child", 15);

        struct sembuf parent_buf =
                {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
        if(semop(semid, &parent_buf, 1) < 0){
            printf("Can\'t sub 1 from semaphor\n");
            exit(-1);
        }
        char result[15] = "\0";
        read(pd[0], result, 15);
        printf("PARENT: %s\n", result);
        sleep(1);
    }
}

int main() {
    if(pipe(fd) < 0) {
        printf("Pipe error\n");
        return -1;
    }

    char pathname[]="pipe-sem.c";
    key_t key = ftok(pathname, 0);
    
    if((semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL) ) < 0){
        printf("Can't create semaphore\n");
        return -1;
    }
    semctl(semid, 0, SETVAL, 1);

    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork error\n");
        return -1;
    } else if (pid == 0) {
        // Child process
        signal(SIGINT, signal_handler);
        child(fd, semid);
    } else {
        // Parent process
        signal(SIGINT, signal_handler);
        parent(fd, semid);
        wait(NULL);

        cleanup();
    }
    return 0;
}



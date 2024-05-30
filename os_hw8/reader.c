// reader.c - читатель забирающий из первой по очереди занятой ячейки
#include "common.h"
#include <unistd.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

const char *reader_sem_name = "/reader-semaphore";
sem_t *reader;

void sigfunc(int sig) {
    if(sig != SIGINT && sig != SIGTERM) {
        return;
    }
    if(sig == SIGINT) {
        kill(buffer->writer_pid, SIGTERM);
        printf("Reader(SIGINT) ---> Writer(SIGTERM)\n");
    } else if(sig == SIGTERM) {
        printf("Reader(SIGTERM) <--- Writer(SIGINT)\n");
    }
    sem_close(reader);
    sem_unlink(reader_sem_name);
    printf("Reader: bye!!!\n");
    exit(10);
}

int main() {
    signal(SIGINT, sigfunc);
    signal(SIGTERM, sigfunc);

    srand(time(0));
    init();

    buf_id = shm_open(shar_object, O_RDWR, 0666);
    ftruncate(buf_id, sizeof(shared_memory));
    buffer = mmap(0, sizeof(shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, buf_id, 0);

    reader = sem_open(reader_sem_name, O_CREAT, 0666, 1);
    sem_wait(reader);
    if(buffer->have_reader) {
        printf("Reader %d: I have lost this work :(\n", getpid());
        sem_post(reader);
        exit(13);
    }
    buffer->have_reader = 1;
    sem_post(reader);
    buffer->reader_pid = getpid();

    while (1) {
        sem_wait(full);
        sem_wait(mutex);
        int next_index = buffer->read_index;
        if (buffer->store[next_index] != -1) {
            int value = buffer->store[next_index];
            printf("Reader %d: Reads value = %d from cell [%d]\n", getpid(), value, next_index);
            buffer->store[next_index] = -1;
            buffer->read_index = (next_index + 1) % BUF_SIZE;
        }
        sem_post(empty);
        sem_post(mutex);
        sleep(rand() % 3 + 1);
    }
}

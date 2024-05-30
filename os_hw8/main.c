// writer.c - писатель, пишущий в следующую по порядку ячейку
#include "common.h"
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

// Семафор для отсекания лишних писаталей
// То есть, для формирования только одного процесса-писателя
// Закрывает критическую секцию для анализа наличия писателей
const char *writer_sem_name = "/writer-semaphore";
sem_t *writer;  // указатель на семафор пропуска писателей
// Семафор, играющий роль флага для допуска писателей
const char *first_writer_sem_name = "/first_writer-semaphore";
sem_t *first_writer; // указатель на семафор допуска

// Функция, осуществляющая обработку сигнала прерывания работы
// Осществляет удаление всех семафоров и памяти. Заодно "убивает" читателя
// независимо от его текущего состояния
void sigfunc(int sig) {
    if(sig != SIGINT && sig != SIGTERM) {
        return;
    }
    if(sig == SIGINT) {
        kill(buffer->reader_pid, SIGTERM);
        printf("Writer(SIGINT) ---> Reader(SIGTERM)\n");
    } else if(sig == SIGTERM) {
        printf("Writer(SIGTERM) <--- Reader(SIGINT)\n");
    }

    // Закрывает видимые семафоры и файлы, доступные процессу
    sem_close(writer);
    sem_close(first_writer);
    close_common_semaphores();
    // Удаляет все свои семафоры и разделяемую память
    sem_unlink(writer_sem_name);
    sem_unlink(first_writer_sem_name);
    unlink_all();
    printf("Writer: bye!!!\n");
    exit(10);
}

int main() {
    // Задание обработчика, завершающего работу писателя и обеспечивающего
    // уничтожение контекста и процессов
    signal(SIGINT, sigfunc);
    signal(SIGTERM, sigfunc);

    srand(time(0)); // Инициализация генератора случайных чисел
    init(); // Начальная инициализация общих семафоров

    // Формирование объекта разделяемой памяти для общего доступа к кольцевому буферу
    buf_id = shm_open(shar_object, O_CREAT|O_RDWR, 0666);
    if (buf_id == -1) {
        perror("shm_open");
        exit(-1);
    }
    // Задание размера объекта памяти
    ftruncate(buf_id, sizeof(shared_memory));

    //получить доступ к памяти
    buffer = mmap(0, sizeof(shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, buf_id, 0);
    if (buffer == MAP_FAILED) {
        perror("writer: mmap");
        exit(-1);
    }

    // Разборки писателей. Семафор для конкуренции за работу
    writer = sem_open(writer_sem_name, O_CREAT, 0666, 1);
    // Дополнительный семафор, играющий роль счетчика-ограничителя
    first_writer = sem_open(first_writer_sem_name, O_CREAT, 0666, 1);

    // Первый просочившийся запрещает доступ остальным за счет установки флага
    if(sem_trywait(first_writer) == -1) {
        printf("Writer %d: I have lost this work :(\n", getpid());
        exit(13);
    }

    sem_wait(writer);
    buffer->writer_pid = getpid();
    for(int i = 0; i < BUF_SIZE; ++i) {
        buffer->store[i] = -1;
    }
    sem_post(writer);

    while (1) {
        sem_wait(empty);
        sem_wait(mutex);

        int next_index = buffer->write_index;
        if (buffer->store[next_index] == -1) {
            buffer->store[next_index] = rand() % 101;
            printf("Writer %d writes value = %d to cell [%d]\n", getpid(), buffer->store[next_index], next_index);
            buffer->write_index = (next_index + 1) % BUF_SIZE;
        }

        sem_post(full);
        sem_post(mutex);
        sleep(rand() % 3 + 1);
    }
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define RCVBUFSIZE 1024   // Размер буфера приема

void DieWithError(char *errorMessage);  // Функция обработки ошибок
void *SendThread(void *arg);            // Поток отправки данных
void *ReceiveThread(void *arg);         // Поток приема данных

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort;
    char *servIP;
    pthread_t sendThreadId, receiveThreadId;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
        exit(1);
    }

    servIP = argv[1];
    echoServPort = atoi(argv[2]);

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port = htons(echoServPort);

    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    // Создание потоков для отправки и приема
    if (pthread_create(&sendThreadId, NULL, SendThread, (void *) &sock) != 0)
        DieWithError("pthread_create() failed for send");
    if (pthread_create(&receiveThreadId, NULL, ReceiveThread, (void *) &sock) != 0)
        DieWithError("pthread_create() failed for receive");

    // Ожидание завершения потоков
    pthread_join(sendThreadId, NULL);
    pthread_join(receiveThreadId, NULL);

    close(sock);
    return 0;
}

void *SendThread(void *arg) {
    int sock = *((int *)arg);
    char echoString[RCVBUFSIZE];

    printf("Connected to server.\n");

    while (fgets(echoString, RCVBUFSIZE, stdin) != NULL) {
        int echoStringLen = strlen(echoString);
        if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
            DieWithError("send() failed");

        if (strcmp(echoString, "The End\n") == 0) break;
    }
    return NULL;
}

void *ReceiveThread(void *arg) {
    int sock = *((int *)arg);
    char echoBuffer[RCVBUFSIZE];
    int bytesRcvd;

    while ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) > 0) {
        echoBuffer[bytesRcvd] = '\0';
        printf("Received: %s\n", echoBuffer);
        if (strcmp(echoBuffer, "The End") == 0) break;
    }
    return NULL;
}

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

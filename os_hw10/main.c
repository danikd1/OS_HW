#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define RCVBUFSIZE 1024

void DieWithError(char *errorMessage);
int AcceptTCPConnection(int servSock);
void HandleTCPClient(int clntSocket1, int clntSocket2);

int main(int argc, char *argv[]) {
    int servSock, clntSock1, clntSock2;
    unsigned short echoServPort;
    struct sockaddr_in echoServAddr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    if (listen(servSock, 2) < 0)
        DieWithError("listen() failed");

    clntSock1 = AcceptTCPConnection(servSock);
    clntSock2 = AcceptTCPConnection(servSock);

    HandleTCPClient(clntSock1, clntSock2);

    close(servSock);
    return 0;
}

void HandleTCPClient(int clntSocket1, int clntSocket2) {
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;
    fd_set readfds;
    int max_sd;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(clntSocket1, &readfds);
        FD_SET(clntSocket2, &readfds);
        max_sd = clntSocket1 > clntSocket2 ? clntSocket1 : clntSocket2;

        if (select(max_sd + 1, &readfds, NULL, NULL, NULL) < 0)
            DieWithError("select() failed");

        if (FD_ISSET(clntSocket1, &readfds)) {
            if ((recvMsgSize = recv(clntSocket1, echoBuffer, RCVBUFSIZE, 0)) < 0)
                DieWithError("recv() failed");

            if (send(clntSocket2, echoBuffer, recvMsgSize, 0) != recvMsgSize)
                DieWithError("send() failed");
        }

        if (FD_ISSET(clntSocket2, &readfds)) {
            if ((recvMsgSize = recv(clntSocket2, echoBuffer, RCVBUFSIZE, 0)) < 0)
                DieWithError("recv() failed");

            if (send(clntSocket1, echoBuffer, recvMsgSize, 0) != recvMsgSize)
                DieWithError("send() failed");
        }
    }
}

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

int AcceptTCPConnection(int servSock) {
    int clntSock;
    struct sockaddr_in echoClntAddr;
    unsigned int clntLen = sizeof(echoClntAddr);

    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
        DieWithError("accept() failed");

    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
    return clntSock;
}


/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * server.c
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    char* buf;
    buf = (char *)malloc(BUFLEN * sizeof(char));
    struct sockaddr_in serv_addr, client_addr;

    if(argc != 3)
    {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // deschidere socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // completare informatii despre adresa serverului
    // inet_aton(argv[1], &(servaddr.sin_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // legare proprietati de socket
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // ascultare de conexiuni
    if (listen(listenfd, 1) != 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // acceptarea unei conexiuni, primirea datelor, trimiterea raspunsului
    unsigned int namelen = sizeof(client_addr);
    if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &namelen)) == -1) {
        perror("accept error");
        exit(EXIT_FAILURE);
    }

    do {
        if (recv(connfd, buf, BUFLEN, 0) < 0) {
            perror("receive error");
            exit(EXIT_FAILURE);
        }
        printf("[client]: %s\n", buf);
        // scanf("%s", buf);
        printf(">");
        size_t size = BUFLEN;
        getline(&buf, &size, stdin);

        if (send(connfd, buf, BUFLEN, 0) < 0) {
            perror("send error");
            exit(EXIT_FAILURE);
        }
    } while (strcmp(buf, "exit\n"));


    // inchidere socket(i)
    close(connfd);
    close(listenfd);

    return 0;
}

/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * client.c
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    int sockfd = 0;
    char* buf;
    buf = (char *)malloc(BUFLEN * sizeof(char));
    struct sockaddr_in serv_addr;

    if(argc != 3)
    {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // deschidere socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        return -1;
    }

    // completare informatii despre adresa serverului
    inet_aton(argv[1], &(serv_addr.sin_addr));
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    // conectare la server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed \n");
        return -1;
    }

    do {
        // scanf("%s", buf);
        printf(">");
        size_t size = BUFLEN;
        getline(&buf, &size, stdin);
        send(sockfd, buf, BUFLEN, 0);
        recv(sockfd, buf, BUFLEN, 0);
        printf("[server]: %s\n", buf);
    } while (strcmp(buf, "exit\n"));
    // citire de la tastatura, trimitere de cereri catre server, asteptare raspuns

    // inchidere socket
    close(sockfd);
    return 0;
}

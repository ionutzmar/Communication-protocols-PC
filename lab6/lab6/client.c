/*
 * Protocoale de comunicatii:
 * Laborator 6: UDP
 * client mini-server de backup fisiere
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "helpers.h"

#define PORT 22222
#define IP "127.0.0.1"
#define MAXLINE 1024

void usage(char *file)
{
	fprintf(stderr, "Usage: %s ip_server port_server file\n", file);
	exit(0);
}

/*
 * Utilizare: ./client ip_server port_server nume_fisier_trimis
 */
int main(int argc, char **argv)
{
	// if (argc != 4)
	// 	usage(argv[0]);

	// int fd, sock;
	// struct sockaddr_in to_station;
	// char buf[BUFLEN];
	// int ret;

	/* TODO deschidere socket */
	int socketfd;
	if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

	/* TODO setare struct sockaddr_in pentru a specifica unde trimit
	 * datele */
	struct sockaddr_in servaddr;
	inet_aton(IP, &(servaddr.sin_addr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    // servaddr.sin_addr.s_addr = INADDR_ANY;


	/* TODO deschidere fișier pentru citire */

	/*
	 * TODO
	 * cat_timp  mai_pot_citi
	 *		citeste din fisier
	 *		trimite pe socket
	 */
	char buffer[MAXLINE];
    char *hello = "Hello from client";
	int n;
	unsigned int len;
	sendto(socketfd, (const char *)hello, strlen(hello),
        0, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));
    printf("[Client] Hello message sent.\n");

    n = recvfrom(socketfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0';
    printf("[client]Server responded with: %s\n", buffer);

	/* TODO închidere socket */
	close(socketfd);

	/* TODO închidere fișier */

	return 0;
}

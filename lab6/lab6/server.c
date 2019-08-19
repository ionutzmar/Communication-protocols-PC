/*
 * Protocoale de comunicatii:
 * Laborator 6: UDP
 * mini-server de backup fisiere
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
#define MAXLINE 1024

void usage(char*file)
{
	fprintf(stderr, "Usage: %s server_port file\n", file);
	exit(0);
}

/*
 * Utilizare: ./server server_port nume_fisier
 */
int main(int argc, char **argv)
{
	// if (argc != 3)
	// 	usage(argv[0]);

	// int fd, sock;
	// socklen_t socklen;
	// struct sockaddr_in from_station;
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
	struct sockaddr_in servaddr, cliaddr;
	memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

	/* TODO legare proprietăți de socket */
	if (bind(socketfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	/* TODO deschidere fișier pentru scriere */

	/*
	 * TODO
	 * cat_timp  mai_pot_citi
	 *		citeste din socket
	 *		pune in fisier
	 */
	char buffer[MAXLINE];
   	char *hello = "Hello from server";
	int n;
	unsigned int len;
	n = recvfrom(socketfd, (char *)buffer, MAXLINE,  0, (struct sockaddr *)&cliaddr, &len);
	if (n < 0) {
		perror("server: recvfrom failed");
    	exit(EXIT_FAILURE);
	}
	buffer[n] = '\0';
	printf("[Server] received: %s\n", buffer);
	sendto(socketfd, (const char *)hello, strlen(hello), 0, (const struct sockaddr *) &cliaddr, len);
	printf("[Server]: Hello message sent.\n");

	/* TODO închidere socket */
	close(socketfd);

	/* TODO închidere fișier */

	return 0;
}

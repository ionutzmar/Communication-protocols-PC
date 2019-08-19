// Protocoale de comunicatii
// Laborator 9 - DNS
// dns.c

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int usage(char* name)
{
	printf("Usage:\n\t%s -n <NAME>\n\t%s -a <IP>\n", name, name);
	return 1;
}

// Receives a name and prints IP addresses
void get_ip(char* name)
{
	int ret;
	struct addrinfo hints, *result, *p;
	char buff[100];

	// TODO: set hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; //AF_UNSPEC
	// hints.ai_socktype = SOCK_STREAM;

	// TODO: get addresses
	if ((ret = getaddrinfo(name, "http", &hints, &result)) != 0) {
	    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
	    exit(1);
	}

	// TODO: iterate through addresses and print them
	for(p = result; p != NULL; p = p->ai_next) {
	    if (p->ai_family == AF_INET) {
			struct sockaddr_in *sockaddr = (struct sockaddr_in *) p->ai_addr;
			if (inet_ntop(AF_INET, &sockaddr->sin_addr, buff, sizeof(buff)) == NULL) {
				perror("inet_ntop() error");
				continue;
			}
			printf("ip: %s: %d\n", buff, htons(sockaddr->sin_port));
		}
	}
	// TODO: free allocated data
	freeaddrinfo(result);
}

// Receives an address and prints the associated name and service
void get_name(char* ip)
{
	int ret;
	struct sockaddr_in addr;
	char host[1024];
	char service[20];

	// TODO: fill in address data
	inet_pton(AF_INET, ip, &addr.sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);

	// TODO: get name and service
	ret = getnameinfo((struct sockaddr*)&addr, sizeof(addr),
	host, sizeof(host),
	service, sizeof(service), NI_NAMEREQD);

	// TODO: print name and service
    if (ret < 0) {
        printf("error: %d\n", ret);
        printf("%s\n", gai_strerror(ret));
    }
    else
        printf("host: %s; service: %s\n", host, service);

}

int main(int argc, char **argv)
{
	if (argc < 3) {
		return usage(argv[0]);
	}

	if (strncmp(argv[1], "-n", 2) == 0) {
		get_ip(argv[2]);
	} else if (strncmp(argv[1], "-a", 2) == 0) {
		get_name(argv[2]);
	} else {
		return usage(argv[0]);
	}

	return 0;
}

#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *url_params)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    /*
        PAS 1: Scriem numele metodei, calea, parametri din url (daca exista) si tipul protocolului
    */
    if (url_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);


    strcat(message, "\r\n");

    return message;
}
char *compute_request(char *protocol, char *host, char *url, char *url_params, char *header, char *form_data)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (url_params != NULL)
        sprintf(line, "%s %s?%s HTTP/1.1", protocol, url, url_params);
    else
        sprintf(line, "%s %s HTTP/1.1", protocol, url);
    compute_message(message, line);

    sprintf(line, "%s\r\n", header);
    compute_message(message, line);

    sprintf(line, "%s", form_data);
    compute_message(message, line);
    strcat(message, "\r\n");
    return message;
}

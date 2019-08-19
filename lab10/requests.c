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

    /*
        PAS 2: Adaugam host-ul
    */
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // TODO Ex 0

    /*
        PAS 3 (optional): Adaugam headere si/ sau cookies, respectand forma protocolului
    */
    // TODO BONUS COOKIE

    /*
        PAS 4: Adaugam linia de final
    */

    //TODO Ex 0
    strcat(message, "\r\n");

    return message;
}
char *compute_post_request(char *host, char *url, char *form_data)
{

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    /*
        PAS 1: Scriem numele metodei, calea si tipul protocolului
    */

    // TODO Ex 2
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    /*
        PAS 2: Adaugam host-ul
    */

    // TODO Ex 2
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /*
        PAS 3: Adaugam headere si/ sau cookies, respectand forma protocolului
        ATENTIE!! Headerele Content-Type si Content-Length sunt obligatorii
    */

    // TODO Ex 2
    sprintf(line, "Content-Type: application/x-www-form-urlencoded");
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", strlen(form_data));
    compute_message(message, line);

    /*
        PAS 4: Adaugam linia de final de antent
    */

    // TODO Ex 2
    strcat(message, "\r\n");

    /*
        PAS 5: Adaugam data
    */
    sprintf(line, "%s", form_data);
    compute_message(message, line);

    // TODO Ex 2
    return message;
}

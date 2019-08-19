#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[])
{
    //folosite de biblioteca parson
    JSON_Value *root_value;
    JSON_Object *resp;

    //variabile refolosite de-a lungul programuui
    char *message;
    char *response;
    int sockfd;
    char header[BUFLEN];
    char urlParams[BUFLEN];
    char formData[BUFLEN];
    char cookie[BUFLEN];
    char *responseBackup;
    bzero(cookie, BUFLEN);

    //Task 1
    message = compute_get_request(HOST, "/task1/start", NULL);
    printf("%s", message);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    printf("%s\n", response);

    //Extragere cookie-uri si formatarea lor in mod corespunzator
    int j = 0;
    responseBackup = response;
    while ((responseBackup = strstr(responseBackup, "Set-Cookie: ")) != NULL) {
        responseBackup += strlen("Set-Cookie: ");
        int i = 0;
        if (j != 0) {
            strcat(cookie, "; ");
            j += 2;
        }
        for (i = 0; responseBackup[i] != '\r'; i++, j++) {
            cookie[j] = responseBackup[i];
        }
    }
    cookie[j] = '\0';

    //extragere date din json ul primit
    char *json = strstr(response, "{");
    root_value = json_parse_string(json);
    resp = json_object(root_value);
    const char *url2 = json_object_dotget_string(resp, "url");
    const char *username2 = json_object_dotget_string(resp, "data.username");
    const char *password2 = json_object_dotget_string(resp, "data.password");
    const char *method2 = json_object_dotget_string(resp, "method");
    const char *type2 = json_object_dotget_string(resp, "type");

    sprintf(formData, "username=%s&password=%s", username2, password2);
    sprintf(header, "Host: %s\r\nCookie: %s\r\nContent-Type: %s\r\nContent-Length: %lu", HOST, cookie, type2, strlen(formData));
    free(message);
    message = compute_request((char *) method2, HOST,(char *) url2, NULL, header, formData);
    printf("%s", message);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    free(response);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    printf("%s\n", response);
    json_value_free(root_value);

    //se extrag noile cookie uri
    j = 0;
    responseBackup = response;
    while ((responseBackup = strstr(responseBackup, "Set-Cookie: ")) != NULL) {
        responseBackup += strlen("Set-Cookie: ");
        int i = 0;
        if (j != 0) {
            cookie[j] = ';';
            cookie[j + 1] = ' ';
            j += 2;
        }
        for (i = 0; responseBackup[i] != '\r'; i++, j++) {
            cookie[j] = responseBackup[i];
        }
    }
    cookie[j] = '\0';

    //extragerea datelor din json pentru task 3
    json = strstr(response, "{");
    root_value = json_parse_string(json);
    resp = json_object(root_value);

    const char *url3 = json_object_dotget_string(resp, "url");
    const char *method3 = json_object_dotget_string(resp, "method");
    const char *jwt3 = json_object_dotget_string(resp, "data.token");
    char *jwt = (char *) malloc(strlen(jwt3) + 1);
    strcpy(jwt, jwt3);
    const char *id3 = json_object_dotget_string(resp, "data.queryParams.id");
    sprintf(header, "Host: %s\r\nCookie: %s\r\nAuthorization: Bearer %s", HOST, cookie, jwt3);
    sprintf(urlParams, "raspuns1=Omul&raspuns2=Numele&id=%s", (char *) id3);
    free(message);
    message = compute_request((char *) method3, HOST,(char *) url3, urlParams, header, "");
    printf("%s", message);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    free(response);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    printf("%s\n", response);
    json_value_free(root_value);

    //extragere cookie uri pentru task 4
    j = 0;
    responseBackup = response;
    while ((responseBackup = strstr(responseBackup, "Set-Cookie: ")) != NULL) {
        responseBackup += strlen("Set-Cookie: ");
        int i = 0;
        if (j != 0) {
            cookie[j] = ';';
            cookie[j + 1] = ' ';
            j += 2;
        }
        for (i = 0; responseBackup[i] != '\r'; i++, j++) {
            cookie[j] = responseBackup[i];
        }
    }
    cookie[j] = '\0';
    json = strstr(response, "{");
    root_value = json_parse_string(json);
    resp = json_object(root_value);
    //extragere date din json pentru task 4
    const char *url4 = json_object_dotget_string(resp, "url");
    const char *method4 = json_object_dotget_string(resp, "method");
    sprintf(header, "Host: %s\r\nCookie: %s\r\nAuthorization: Bearer %s", HOST, cookie, jwt);
    free(message);
    message = compute_request((char *) method4, HOST,(char *) url4, NULL, header, "");
    printf("%s", message);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    free(response);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    printf("%s\n", response);
    json_value_free(root_value);
    //extragere cookie uri pentru task 5
    j = 0;
    responseBackup = response;
    while ((responseBackup = strstr(responseBackup, "Set-Cookie: ")) != NULL) {
        responseBackup += strlen("Set-Cookie: ");
        int i = 0;
        if (j != 0) {
            cookie[j] = ';';
            cookie[j + 1] = ' ';
            j += 2;
        }
        for (i = 0; responseBackup[i] != '\r'; i++, j++) {
            cookie[j] = responseBackup[i];
        }
    }
    cookie[j] = '\0';

    //extragere date din json pentru task 5
    json = strstr(response, "{");
    root_value = json_parse_string(json);
    resp = json_object(root_value);

    const char *url5 = json_object_dotget_string(resp, "url");
    const char *method5 = json_object_dotget_string(resp, "method");
    const char *type5 = json_object_dotget_string(resp, "type");
    const char *openWeather = json_object_dotget_string(resp, "data.url");
    const char *weatherMethod = json_object_dotget_string(resp, "data.method");
    const char *q = json_object_dotget_string(resp, "data.queryParams.q");
    const char *appid = json_object_dotget_string(resp, "data.queryParams.APPID");
    char *openWeatherAddress = (char *) malloc(strlen(openWeather) + 1);
    strcpy(openWeatherAddress, openWeather);
    for (j = 0; j < strlen(openWeatherAddress); j++) {
        if(openWeatherAddress[j] == '/') {
            openWeatherAddress[j] = '\0';
            break;
        }
    }
    char *openWeatherIp = get_ip(openWeatherAddress);
    openWeatherAddress[j] = '/';
    char *openWeatherUrl = openWeatherAddress + j;
    sprintf(header, "Host: %s\r\n", openWeatherIp);
    sprintf(urlParams, "q=%s&APPID=%s", (char *) q, (char *) appid);
    free(message);
    //mesajul pentru requestul catre openweathermap
    message = compute_request((char *)  weatherMethod, openWeatherIp, openWeatherUrl, urlParams, header, "");
    printf("%s", message);
    //conectare la openweathermap
    sockfd = open_connection(openWeatherIp, 80, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    free(response);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    printf("%s\n", response);

    //json ul din raspunsul de la openweathermap
    json = strstr(response, "{");
    sprintf(header, "Host: %s\r\nCookie: %s\r\nContent-Type: %s\r\nContent-Length: %lu\r\nAuthorization: Bearer %s", HOST, cookie, type5, strlen(json), jwt);
    sprintf(urlParams, "q=%s&APPID=%s", (char *) q, (char *) appid);
    free(message);
    message = compute_request((char *)  method5, HOST, (char *) url5, NULL, header, json);
    printf("%s", message);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    free(response);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    printf("%s\n", response);
    json_value_free(root_value);

    free(openWeatherIp);
    free(openWeatherAddress);
    free(jwt);
    free(message);
    free(response);
    return 0;
}


/*
MIHALESCU MIHAELA-VIOLETA
324CB
TEMA1 - PROTOCOALE DE COMUNICATIE
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001
#define SIZE 1360
#define MAX 4000

typedef struct
{
    int number;
    char content[SIZE];
    char corrupt;
} message;

char is_corrupt(char *payload, size_t size)
{
    char result = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        result ^= payload[i];
    }
    return result;
}

int check_if_corrupt(msg m)
{
    message t;
    memcpy(&t, m.payload, sizeof(message));
    int *n = &(t.number);
    char *p = (char *)n;
    char result = (*(p)) ^ (*(p + 1));
    result = result ^ (*(p + 2));
    result = result ^ (*(p + 3));
    result = result ^ is_corrupt(t.content, SIZE); // TODO
    if (result == t.corrupt)
    {
        return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    char ACK[] = "ACK";
    char NACK[] = "NACK";
    char file_name[100];
    msg m, ack;
    message t;
    msg *messages;
    int corrupt;
    int res;
    int COUNT = 0;
    init(HOST, PORT);
    messages = calloc(MAX, sizeof(msg));
    if (!messages)
    {
        perror("calloc error");
        exit(1);
    }
    int number_of_acks = 0;

    while (1)
    {
        if (number_of_acks == COUNT && COUNT != 0)
        {
            break;
        }
        res = recv_message(&m); // astept mesaj
        if (res < 0)
        {
            perror("[RECEIVER] Receive error. Exiting.\n");
            return -1;
        }
        corrupt = check_if_corrupt(m);
        if (corrupt == 1)
        {
            if (t.number < COUNT)
            {
                memcpy(ack.payload, NACK, sizeof(NACK) + 1);
                ack.len = t.number;
                res = send_message(&ack);
                if (res < 0)
                {
                    perror("[RECEIVER] Send ACK error. Exiting.\n");
                    return -1;
                }
            }
            continue;
        }
        memcpy(&t, m.payload, sizeof(t));
        if (messages[t.number].len == 0)
        {
            if (t.number == 0)
            {
                COUNT = m.len;
                m.len = strlen(t.content) + 1;
                memcpy(file_name, t.content, strlen(t.content) + 1);
            }
            memcpy(messages + t.number, &m, sizeof(msg));
            number_of_acks++;
            memcpy(ack.payload, ACK, sizeof(ACK) + 1);
            ack.len = t.number;
            res = send_message(&ack);
            if (res < 0)
            {
                perror("[RECEIVER] Send ACK error. Exiting.\n");
                return -1;
            }
        }
        else
        {
            continue;
        }
    }

    int fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0777);
    if (fd < 0)
    {
        perror("recv open");
        exit(-1);
    }

    int j;
    for (j = 1; j < COUNT; j++)
    {
        memcpy(&m, messages + j, sizeof(msg));
        memcpy(&t, m.payload, sizeof(message));
        write(fd, t.content, m.len);
    }

    close(fd);
    free(messages);
    ack.len = -9;
    res = send_message(&ack);
    if (res < 0)
    {
        perror("[RECEIVER] Send ACK error. Exiting.\n");
        return -1;
    }
    return 0;
}

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
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000
#define SIZE 1360
#define DELAY atoi(argv[3])

typedef struct
{
    int number;
    char content[SIZE];
    char corrupt;
} message;

char is_corrupt(char *payload, size_t size)
{
    int i;
    char result = 0;
    for (i = 0; i < size; i++)
    {
        result ^= payload[i];
    }
    return result;
}

void set_corrupt(message *t)
{
    int *n = &(t->number);
    char *p = (char *)n;
    char result = (*(p)) ^ (*(p + 1)) ^ (*(p + 2)) ^ (*(p + 3));
    result = result ^ is_corrupt(t->content, SIZE); // TODO
    t->corrupt = result;
}

int get_next_index(msg *messages, int message_to_send, int COUNT)
{
    do
    {
        message_to_send++;
        if (message_to_send == COUNT + 1)
        {
            message_to_send = 0;
        }
    } while (messages[message_to_send].len == -1);
    return message_to_send;
}

int main(int argc, char **argv)
{
    char ACK[] = "ACK";
    char NACK[] = "NACK";
    message t;
    msg m;
    init(HOST, PORT);
    msg ack;
    msg *messages;
    int window= (atol(argv[2]) * atol(argv[3]) * 1000) / (sizeof(msg) * 8);
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("Open file error");
        exit(-1);
    }
    int file_size = lseek(fd, 0, SEEK_END);

    lseek(fd, 0, SEEK_SET);
    int res;
    int COUNT = file_size / (SIZE) + 1; 
    if (COUNT < window)
    {
        window= COUNT;
    }
    messages = calloc(COUNT + 10, sizeof(msg));
    if (!messages)
    {
        perror("calloc error");
        exit(1);
    }

    char name[] = "recv_fileX"; // SEND NAME
    t.number = 0;
    memcpy(t.content, name, strlen(name) + 1);
    set_corrupt(&t);
    memcpy(m.payload, &t, sizeof(message));
    m.len = COUNT + 1;
    send_message(&m); // check
    window--;

    int number = 1;
    ssize_t bytesRead;
    while ((bytesRead = read(fd, t.content, sizeof(t.content))) > 0)
    {
        t.number = number;
        set_corrupt(&t);
        m.len = bytesRead;
        memcpy(m.payload, &t, sizeof(message));
        memcpy(messages + number, &m, sizeof(msg));
        number++;
    }

    int message_to_send = 0;
    int number_of_acks = 0;

    while (1)
    {
        while ((window> 0) && (number_of_acks <= COUNT))
        {
            message_to_send = get_next_index(messages, message_to_send, COUNT);
            res = send_message(messages + message_to_send);
            if (res < 0)
            {
                perror("[SENDER] Send error. Exiting.\n");
                return -1;
            }
            window--;
        }
        window++;
        if (number_of_acks == COUNT + 1)
        {
            break;
        }
        res = recv_message_timeout(&ack, 2 * DELAY);
        if (res < 0)
        {
            message_to_send = get_next_index(messages, message_to_send, COUNT);
            res = send_message(messages + message_to_send);
            if (res < 0)
            {
                perror("[SENDER] Send error. Exiting.\n");
                return -1;
            }
            window++;
        }
        else
        {
            if (!strcmp(ack.payload, ACK))
            {
                if (ack.len >= 0 && ack.len <= COUNT && messages[ack.len].len != -1)
                {
                    number_of_acks++;
                    messages[ack.len].len = -1;
                }
            }
            else if (!strcmp(ack.payload, NACK))
            {
                res = send_message(messages + ack.len);
                if (res < 0)
                {
                    perror("[SENDER] Send error. Exiting.\n");
                    return -1;
                }
            }
            window++;
        }
    }

    recv_message_timeout(&ack, 5 * DELAY);
    free(messages);
    close(fd);
    return 0;
}

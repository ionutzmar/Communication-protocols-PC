#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <set>
#include <iterator>
#include <math.h>

using namespace std;

#define BUFLEN 1600
#define MAX_CLIENTS 128

typedef struct subscribeMessage {
    bool type; //0 = unsubscribe; 1 = subscribe
    char topic[256];
    bool sf; //store&forward
} subscribeMessage;

typedef struct clientStatus {
    bool connected = false;
    int fd;
    string clientId;
} clientStatus;

void usage(char *file)
{
	fprintf(stderr, "Usage: %s <server_port>\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int tcpsock, newsockfd, portno, udpsock;
	char buffer[BUFLEN];
	char toSend[BUFLEN];
	struct sockaddr_in serv_addr, cli_addr;
	int n, i, ret;
	socklen_t clilen;

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

    map<int, string> clientIds; //corelatie intre file descriptor si clientId
    map<string, clientStatus> clientIdsMap; //corelatie intre clientId si o structura cu care tinem evidenta clientilor conectati
    map<string, map<string, bool>> topics; //<topic <cliendId, store&forward>>
    multimap <string, string> missedNotification; // <clientId, notificare>

	if (argc < 2) {
		usage(argv[0]);
	}

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //tcp socket
	tcpsock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpsock < 0) {
        perror("socket creation error");
        exit(1);
    }
	ret = bind(tcpsock, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	if (ret < 0) {
        perror("bind error");
        exit(1);
    }
	ret = listen(tcpsock, MAX_CLIENTS);
	if (ret < 0) {
        perror("listen error");
        exit(1);
    }

    //udp socket
    udpsock = socket(AF_INET, SOCK_DGRAM, 0);
    bind(udpsock, (struct sockaddr*) &serv_addr, sizeof(struct sockaddr));

    // se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	// se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(tcpsock, &read_fds);
	FD_SET(udpsock, &read_fds);
	FD_SET(0, &read_fds);
	fdmax = max(tcpsock, udpsock);

	while (1) {
		tmp_fds = read_fds;

		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		if (ret < 0) {
            perror("select error");
            exit(1);
        }

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
                if (i == 0) { //tastatura
                    char inputMessage[5];
                    cin >> inputMessage;
                    if (strcmp(inputMessage, "exit") == 0) {
                        for (i = 3; i < fdmax; i++) { //fara stdin, stdout si stderr
                            if (FD_ISSET(i, &tmp_fds)) {
                                close(i);
                            }
                        }
                        exit(0);
                    }
                }
                else if (i == udpsock) {
                    socklen_t len = sizeof(cli_addr);
                    bzero(buffer, sizeof(buffer));
                    recvfrom(udpsock, buffer, sizeof(buffer), 0,
                         (struct sockaddr*)&cli_addr, &len);
                    //parsarea mesajului:
                    char topic[51];
                    memcpy(topic, buffer, 50);
                    topic[50] = 0;
                    int contentType = buffer[50];
                    char content[1501];
                    memcpy(content, buffer + 51, 1500);
                    content[1500] = 0;
                    string contentTypeString;
                    char value[1501];
                    memset(value, 0, 1501);
                    switch(contentType) {
                        case 0: {
                                contentTypeString = "INT";
                                uint32_t val;
                                memcpy(&val, content + 1, 4);
                                val = ntohl(val);
                                sprintf(value, "%s%u", (content[0] == 1 ? "-" : ""), val);
                            }
                            break;
                        case 1: {
                                contentTypeString = "SHORT_REAL";
                                uint16_t val;
                                memcpy(&val, content, 2);
                                val = ntohs(val);
                                sprintf(value, "%u.%s%u", val / 100, (val % 100 < 10 ? "0" : ""), val % 100);
                            }
                            break;
                        case 2: {
                                contentTypeString = "FLOAT";
                                uint32_t val;
                                memcpy(&val, content + 1, 4);
                                val = ntohl(val);
                                uint8_t zecimalValue;
                                memcpy(&zecimalValue, content + 5, 1);
                                double number = (double) val;
                                number = number / pow(10, zecimalValue);
                                sprintf(value, "%s%f", (content[0] == 1 ? "-" : ""), number);
                                bool hasZero = false;
                                for (int i = 1500; i > 0; i--) {
                                    if (value[i] == '0') {
                                        hasZero = true;
                                    } else if (hasZero) {
                                        value[i + 1] = '\0';
                                        if (value[i] == '.')
                                            value[i] = 0;
                                        break;
                                    }
                                }
                            }
                            break;
                        case 3:
                            contentTypeString = "STRING";
                            memcpy(value, content, 1501);
                            break;
                    }

                    //construiesc mesajul in toSend
                    bzero(toSend, BUFLEN);
                    sprintf(toSend, "%s:%d - %s - %s - %s\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), topic, contentTypeString.c_str(), value);

                    for (auto client : topics[topic]) {
                        string clientId = client.first;
                        bool storeAndForward = client.second;
                        if (clientIdsMap[clientId].connected) {
                            n = send(clientIdsMap[clientId].fd, &toSend, sizeof(toSend), 0);
                            if (n < 0) {
                                perror("send error");
                                exit(1);
                            }
                        } else if (storeAndForward) {
                            missedNotification.insert(pair <string, string> (clientId, toSend));
                        }
                    }
                }
				else if (i == tcpsock) {
					// a venit o cerere de conexiune pe socketul inactiv (cel cu listen),
					// pe care serverul o accepta
					clilen = sizeof(cli_addr);
					newsockfd = accept(tcpsock, (struct sockaddr *) &cli_addr, &clilen);
					if (newsockfd < 0) {
                        perror("accept error");
                        exit(1);
                    }

					// se adauga noul socket intors de accept() la multimea descriptorilor de citire
					FD_SET(newsockfd, &read_fds);
					if (newsockfd > fdmax) {
						fdmax = newsockfd;
					}

                    memset(buffer, 0, BUFLEN);
					n = recv(newsockfd, buffer, sizeof(buffer), 0);
                    if (n < 0) {
                        perror("recv error");
                        exit(1);
                    }
                    cout << "New client " << buffer << " connected from " << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port) << ".\n";
                    clientIds[newsockfd] = buffer;
                    clientStatus cls;
                    cls.connected = true;
                    cls.fd = newsockfd;
                    cls.clientId = buffer;
                    clientIdsMap[buffer] = cls;

                    //primeste notificarile trimise cat a fost offline si a avut store&forward setat
                    multimap <string, string> :: iterator itr;
                    while ((itr = missedNotification.find(buffer)) != missedNotification.end()) {
                        char message[BUFLEN];
                        strcpy(message, (itr->second).c_str());
                        n = send(newsockfd, message, sizeof(message), 0);
                        if (n < 0) {
                            perror("send error");
                            exit(1);
                        }
                        missedNotification.erase(itr);
                    }
				} else {
					subscribeMessage msg;
					n = recv(i, &msg, sizeof(msg), 0);
                    if (n < 0) {
                        perror("recv error");
                        exit(1);
                    }
					if (n == 0) {
						// conexiunea s-a inchis
						cout << "Client " << clientIds[i] << " disconnected.\n";
                        clientIdsMap[clientIds[i]].connected = false;
                        clientIds.erase(i);
						close(i);
						// se scoate din multimea de citire socketul inchis
						FD_CLR(i, &read_fds);
					} else { //mesaj de subscribe/unsubscribe primit de la un client
                        if (msg.type == true) { //subscribe
                            topics[msg.topic][clientIds[i]] = msg.sf;
                        } else {
                            topics[msg.topic].erase(clientIds[i]);
                        }
					}
				}
			}
		}
	}
	return 0;
}

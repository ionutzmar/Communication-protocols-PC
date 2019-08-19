#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <bits/stdc++.h>

using namespace std;

#define BUFLEN 1600 //to change

typedef struct subscribeMessage {
    bool type; //0 = unsubscribe; 1 = subscribe
    char topic[256];
    bool sf; //store&forward
} subscribeMessage;

void usage(char *file)
{
    cerr << "Usage: " << file << " <client_id> <server_ip> <server_port>\n";
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n, ret, i;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];

	//me
	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	if (argc < 4) {
		usage(argv[0]);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation error");
        exit(1);
    }

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
    if (ret < 0) {
        perror("inet_aton error");
        exit(1);
    }

	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        perror("connection error");
        exit(1);
    }

    n = send(sockfd, argv[1], strlen(argv[1]), 0); //trimitem client_id
    if (n < 0) {
        perror("send error");
        exit(1);
    }

	FD_SET(0, &read_fds);
	FD_SET(sockfd, &read_fds);
	fdmax = sockfd;

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
					// se citeste de la tastatura
					string inputStr;
                    getline(cin, inputStr);

					if (inputStr.compare("exit") == 0) {
						close(sockfd);
                        exit(0);
					}

                    stringstream check1(inputStr);
                    string intermediate;
                    vector <string> tokens;
                    while (getline(check1, intermediate, ' ')) {
                        tokens.push_back(intermediate);
                    }

                    subscribeMessage msg;
                    if (tokens[0].compare("subscribe") == 0) {
                        msg.type = true;
                        msg.sf = atoi(tokens[2].c_str()) == 1 ? true : false;
                    } else {
                        msg.type = false;
                    }
                    strcpy(msg.topic, tokens[1].c_str());

					// se trimite mesaj la server
					n = send(sockfd, &msg, sizeof(msg), 0);
                    if (n < 0) {
                        perror("send error");
                        exit(1);
                    }
				} else {
					memset(buffer, 0, BUFLEN);
					n = recv(sockfd, buffer, sizeof(buffer), 0);
                    if (n < 0) {
                        perror("recv error");
                        exit(1);
                    }
                    if (n == 0) {
                        close(sockfd);
                        exit(0);
                    }
					cout << buffer;
				}
			}
		}
	}

	close(sockfd);

	return 0;
}

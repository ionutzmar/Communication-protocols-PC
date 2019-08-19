#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

typedef struct {
  int type;
  int ack;
  char payload[1392];
} my_pkt;

int main(int argc, char *argv[])
{
	printf("BDP: %s\n", argv[1]);
	int windowSize = (atoi(argv[1]) / (sizeof(msg) * 8));
	printf("Window size: %d\n", windowSize);
	msg t;
	int i, res;
	int n = windowSize * 4 + 3;

	printf("[SENDER] Starting.\n");
	init(HOST, PORT);

	// int numSend;
	//
	// int acks[WINDOW_SIZE];
	// for (i = 0; i < WINDOW_SIZE; i++) {
	// 	acks[i] = 0;
	// }

	for (i = 0; i < windowSize; i++) {
		memset(&t, 0, sizeof(msg));
		t.len = MSGSIZE;
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	for (i = 0; i < n - windowSize; i++) {
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}

		memset(&t, 0, sizeof(msg));
		t.len = MSGSIZE;
		res = send_message(&t);
		if (res < 0) {
			perror("[SENDER] Send error. Exiting.\n");
			return -1;
		}
	}

	for (i = 0; i < windowSize; i++) {
		res = recv_message(&t);
		if (res < 0) {
			perror("[SENDER] Receive error. Exiting.\n");
			return -1;
		}
	}

	// /* printf("[SENDER]: BDP=%d\n", atoi(argv[1])); */
	//
	// for (i = 0; i < COUNT; i++) {
	// 	/* cleanup msg */
	// 	memset(&t, 0, sizeof(msg));
	//
	// 	/* gonna send an empty msg */
	// 	t.len = MSGSIZE;
	//
	// 	/* send msg */
	// 	res = send_message(&t);
	// 	if (res < 0) {
	// 		perror("[SENDER] Send error. Exiting.\n");
	// 		return -1;
	// 	}
	//
	// 	/* wait for ACK */
	// 	res = recv_message(&t);
	// 	if (res < 0) {
	// 		perror("[SENDER] Receive error. Exiting.\n");
	// 		return -1;
	// 	}
	// }

	printf("[SENDER] Job done, all sent.\n");

	return 0;
}

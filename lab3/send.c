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

typedef struct {
  int parity;
  char payload[1396];
} my_pkt;

int main(int argc,char** argv){
  init(HOST,PORT);
  msg t;

  my_pkt m;
  sprintf(m.payload,"Helloa World of PC");

  int sum = 0;
  int i;
  for (i = 0; i < strlen(m.payload); i++) {
    int j;
    for (j = 0; j < 8; j++) {
      sum ^= ((1 << j) & m.payload[i]) >> j;
    }
  }
  m.parity = sum;
  memcpy(t.payload, &m, sizeof(m));
  printf("Sum: %d\n", sum);
  t.len = strlen(m.payload)+4;
  send_message(&t);

  if (recv_message(&t)<0){
    perror("receive error");
  }
  else {
    printf("[%s] Got reply with payload: %s\n",argv[0],t.payload);
  }

  return 0;
}

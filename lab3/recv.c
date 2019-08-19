#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

typedef struct {
  int parity;
  char payload[1396];
} my_pkt;

int main(int argc,char** argv){
  msg r,t;
  init(HOST,PORT);

  if (recv_message(&r)<0){
    perror("Receive message");
    return -1;
  }

  my_pkt m;
  m = *(my_pkt *)r.payload;

  int sum = 0;
  int i;
  for (i = 0; i < strlen(m.payload); i++) {
    int j;
    for (j = 0; j < 8; j++) {
      sum ^= ((1 << j) & m.payload[i]) >> j;
    }
  }
  printf("Errors occured: %d\n", !(sum == m.parity));
  printf("[%s] Got msg with payload: %s\n",argv[0],m.payload);

  sprintf(t.payload,"ACK");
  t.len = strlen(t.payload+1);
  send_message(&t);
  return 0;
}

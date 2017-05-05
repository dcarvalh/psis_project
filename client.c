#include "message.h"

#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>


int void main (){

//int gallery_connect(char *host, in_port_t port);

int sock_fd;                      //UDP Socket that will comunicate with the gateway
struct sockaddr_in local_addr;
struct sockaddr_in gateway_addr;
struct sockaddr_in peer_addr;
socklen_t size_addr;
char *buff;
int nbytes;
message m;

//Socket that comunicates with Gateway
  int sock_fd =socket(AF_INET, SOCK_DGRAM,0);
  if(sock_fd==-1){
    perror("Socket:");
    exit(-1);
  }

  //Incialização da local address
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(2999);
  local_addr.sin_addr.s_addr=INADDR_ANY;

  //Incialização da local address
  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = htons(3001);
  gateway_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

  int err = bind(sock_fd,(struct sockaddr *)&local_addr, sizeof(local_addr));
  if (err==-1){
    perror("Binding:");
    exit(-1);
  }

  printf("Datagram socket created and binded\n");

  size_addr = sizeof(client_addr);

  buff =(char *) malloc(sizeof (m));
  memcpy(buff, &m, sizeof(m));

  //Enviar mensagem para gateway
  nbytes = sendto(sock_fd, buff, sizeof(m), 0,
                	  (const struct sockaddr *) &gateway_addr,sizeof(gateway_addr));

  printf("bytes sent: %d \n", nbytes);

  close(sock_fd);

  printf("OK \n");

  exit(0);
}

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
#define MAX_WAIT_LIST 5

int main (){
  struct sockaddr_in local_addr;
  struct sockaddr_in gateway_addr;
	struct sockaddr_in client_addr;
  socklen_t addrlen;
  char *buff;
  int nbytes;
  message m;

//Socket that comunicates with Gateway
  int sock_gateway_fd = socket(AF_INET, SOCK_DGRAM,0);
  if(sock_gateway_fd==-1){
    perror("Socket:");
    exit(-1);
  }

  //Incialização da local address
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(3000);
  local_addr.sin_addr.s_addr=INADDR_ANY;

  //Incialização da gatewa address
  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = htons(3002);
  gateway_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

  //criar socket TPC e bind
  new_cli_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (new_cli_sock == -1){
    perror("error creating socket \n ");
    exit(-1);
  }

  int err = bind(sock_gateway_fd,(struct sockaddr *)&local_addr, sizeof(local_addr));
  if (err==-1){
    perror("Binding:");
    exit(-1);
  }

  printf("Datagram socket created and binded\n");

  m.port=3001;
  m.message_type= 1;
  sprintf(m.addr,"127.0.0.1");

  printf("\nSent:\n");
  printf("%d \n", m.message_type);
  printf("%s \n", m.addr);
  printf("%d \n\n", m.port);


//  size_addr = sizeof(client_addr);

  buff =(char *) malloc(sizeof (m));
  memcpy(buff, &m, sizeof(m));

  //Enviar mensagem para gateway
  nbytes = sendto(sock_gateway_fd, buff, sizeof(m), 0,
                	  (const struct sockaddr *) &gateway_addr,sizeof(gateway_addr));

  printf("bytes sent: %d \n", nbytes);

  close(sock_gateway_fd);
  free(buff);

  //TCP
  listen(local_addr, MAX_WAIT_LIST);

  char buffer[20];

  while(1){

    int new_cli_sock = accept(local_addr, (struct sockaddr *) & client_addr, &addrlen);
    printf("aceite!\n");
    //Read message
    recv(new_cli_sock, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);
    //Sends story
    fgets
    send(new_cli_sock, buffer, sizeof(buffer), 0);
    close(new_cli_sock);


  }



  return 0;
}

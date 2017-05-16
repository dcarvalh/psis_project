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


int main (){

//int gallery_connect(char *host, in_port_t port);

int sock_fd_gateway;                      //UDP Socket that will comunicate with the gateway
struct sockaddr_in local_addr;
struct sockaddr_in gateway_addr;
//struct sockaddr_in peer_addr;
socklen_t size_addr;
char *buff;
int nbytes;
message m;

//Socket that comunicates with Gateway
  sock_fd_gateway =socket(AF_INET, SOCK_DGRAM,0);
  if(sock_fd_gateway==-1){
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

  int err = bind(sock_fd_gateway,(struct sockaddr *)&local_addr, sizeof(local_addr));
  if (err==-1){
    perror("Binding:");
    exit(-1);
  }

  printf("Datagram socket created and binded\n");

  size_addr = sizeof(gateway_addr);

  //Message of type 0 that tells the gateway that th client is looking for a server
  m.message_type = 0;

  buff =(char *) malloc(sizeof (m));
  memcpy(buff, &m, sizeof(m));

  //Enviar mensagem para gateway
  nbytes = sendto(sock_fd_gateway, buff, sizeof(m), 0,
                	  (struct sockaddr *) &gateway_addr,size_addr);
  if(nbytes == -1){
    perror("Sending");
    exit(-1);
  }

  nbytes = recv(sock_fd_gateway, buff ,sizeof(m), 0);
  if(nbytes == -1){
    perror("Reciving");
    exit(-1);
  }


  memcpy(&m, buff, sizeof(m));
  free(buff);

  if (m.message_type==0){
    printf("Peer recived:\n");
    printf("%s \n", m.addr);
    printf("%d \n\n", m.port);
  }else{
    printf("No peers available :(\n");
  }

///////////////////////////////////////////////////////////////////////////////

  /*TCP connection */
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  // these values can be read from the keyboard
  server_addr.sin_port= htons(m.port);
  inet_aton(m.addr, &server_addr.sin_addr);

  int sock_fd_server= socket(AF_INET, SOCK_STREAM, 0);
  if(sock_fd_server == -1){
    perror("error creating socket \n");
  	exit(-1);
  }

  if( -1 == connect(sock_fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) ){
    printf("Error connecting\n");
    exit(-1);
  }

  printf("TCP socket created and sucsessefully connected\n");

  char buffer[MESSAGE_LEN];
  fgets(buffer, MESSAGE_LEN, stdin);

  send(sock_fd_server, buffer, sizeof(buffer), 0);
  //receive story
  recv(sock_fd_server, buffer, sizeof(buffer), 0);

  printf("%s\n",buffer);

  close(sock_fd_gateway);
  close(sock_fd_server);
  exit(0);
}

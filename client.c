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
#include "API.h"


int main (){

  int peer_socket;
  char * host = "127.0.0.1";
  int port = 3001;

  peer_socket = gallery_connect(host, (in_port_t) htons(port));

  if( (peer_socket == 0) || (peer_socket == -1) ){
    exit(0);
  }


  while(1){
    char buffer[MESSAGE_LEN];
    fgets(buffer, MESSAGE_LEN, stdin);

    send(peer_socket, buffer, sizeof(buffer), 0);
    //receive story
    recv(peer_socket, buffer, sizeof(buffer), 0);

    printf("%s\n",buffer);
  }
  close(peer_socket);
  exit(0);
}

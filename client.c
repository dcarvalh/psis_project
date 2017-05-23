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
  if (peer_socket == 0){
    perror("Peer Connection failled");
    exit(0);
  }

  uint32_t photo_add;
  char image_name[MESSAGE_LEN];//Nome do ficheiro a enviar
  //char image_path[MESSAGE_LEN];

  while(1){
    if(fgets(image_name, MESSAGE_LEN, stdin)==NULL){
      printf("No Inpt\n");
    }else{
      int i= sizeof(image_name);
      printf("Sizeof name: %d\n", i);

      photo_add = gallery_add_photo(peer_socket, (char *)image_name);
    }
    printf("%d\n",photo_add);
  }
  close(peer_socket);
  exit(0);
}

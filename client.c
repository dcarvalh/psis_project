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
        perror("Peer Connection failled");
    exit(0);
  }

  uint32_t photo_add;
  char image_name[MESSAGE_LEN];//Nome do ficheiro a enviar
  char input [MESSAGE_LEN];
  char command;

  printf("Choose one of the following commands\nAdd photo - a\nCommand: ");

  while(1){
    //Getting input command
    fgets(input, MESSAGE_LEN, stdin);
    sscanf(input,"%c",&command);

    //Add-Photo
    if(command=='a'){
      printf("Input the name of the image you want to send\nImage name: ");
      fgets(input, MESSAGE_LEN, stdin);
      sscanf(input,"%s",image_name);
      photo_add = gallery_add_photo(peer_socket, image_name);
      printf("Your photo ID is: %d\n\n",photo_add);
      printf("Choose one of the following commands\nAdd photo - a\nCommand: ");
    }


  }
  close(peer_socket);
  exit(0);
}

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
#include <netinet/in.h>
#include <sys/stat.h>

int gallery_connect(char * host, in_port_t port){

  struct sockaddr_in local_addr;
  struct sockaddr_in gateway_addr;
  //struct sockaddr_in peer_addr;
  socklen_t size_addr;
  char *buff;
  int nbytes;
  message m;
  int sock_fd_gateway;                      //UDP Socket that will comunicate with the gateway


  //Socket that comunicates with Gateway
  sock_fd_gateway =socket(AF_INET, SOCK_DGRAM,0);
  if(sock_fd_gateway==-1){
    perror("Socket:");
    exit(-1);
  }

  //Incialização da local address
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(3009);
  local_addr.sin_addr.s_addr=INADDR_ANY;

  //Incialização da local address
  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = port;
  gateway_addr.sin_addr.s_addr=inet_addr(host);

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
    return 0;
  }

  close(sock_fd_gateway);


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

  if(connect(sock_fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
    printf("Error connecting\n");
    exit(-1);
  }

  printf("TCP socket created and sucsessefully connected\n");

  return sock_fd_server;
}

uint32_t  gallery_add_photo(int peer_socket, char *file_name){

  uint32_t foto_id;
  //strcpy(image_path, "./");

  strcpy(file_name, "space.jpg");

  FILE *picture;
  long pic_size;

  //strcat(image_path, file_name);

  printf("Image name %s\n", file_name);
  picture=fopen(file_name, "rb");
  if(picture==NULL){
    perror("Filename:");
    return 0;
  }

  //Searching the beggining and end of the picture
  fseek(picture, 0, SEEK_END);
  pic_size = ftell(picture);
  rewind(picture);

  //Sending Picture Size, and name to peer
  pic_info p;
  p.message_type = 2;
  p.size = pic_size;
  strcpy(p.pic_name, file_name);

  printf("Image message type: %d\n",p.message_type);
  printf("Image size %ld\n",pic_size);
  printf("Image name %s\n",p.pic_name);
  //Copying picture info to memory and sending it to peer
  char *buff =(char *) malloc(sizeof (p));
  memcpy(buff, &p, sizeof(p));

  int nbytes = send(peer_socket, buff, sizeof(p), 0);
  if(nbytes == -1){
    perror("Sending:");
    exit(0);
  }

  printf("Picture Size sent\n");

  //Sending Picture as byte array
  char send_buffer[p.size];
  size_t fr;
  printf("Sending byte stream\n");
  int check=0;
  while(!feof(picture)){  //Reading file, while it is not the end of file
    printf("Check %d\n", check);
    fr=fread(send_buffer ,sizeof(char), sizeof(send_buffer), picture);
    if(fr>0){
      nbytes = send(peer_socket, send_buffer, sizeof(send_buffer), 0);
      printf("Sending!\n");
      if(nbytes == -1){
        perror("Sending:");
        exit(0);
      }
    }
    bzero(send_buffer, sizeof(send_buffer));
    check++;
  }

  //Reciving photo ID from the Peer
  nbytes = recv(peer_socket, buff, sizeof(p), 0);
  if(nbytes==-1){
    perror("Reciving:");
    exit(0);
  }
  memcpy(&foto_id, buff, sizeof(uint32_t));

  printf("Photo ID: %d", foto_id);
  return foto_id;

}//End of Add Photo

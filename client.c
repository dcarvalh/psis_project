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
#include <signal.h>

//Variables that are used in the signal handlelign function
int end =1;
//Signal handlelling function
static void handle(int sig, siginfo_t *siginfo,void *context);

int main (){

  char * host = "127.0.0.1";
  int port = 3000+getpid();
  pic_info p;
  int peer_socket;
  struct sigaction *act;

////Sigaction Initialization
  act  = malloc(sizeof(act));
  act->sa_sigaction = &handle;
  act->sa_flags = SA_SIGINFO;
//Defining Ctrl+C as a save closing method
  if(sigaction(SIGINT, act, NULL) <0){
    perror("Sigaction:");
  }


  //Connecting client to a peer through the gateway
  peer_socket = gallery_connect(host, (in_port_t) htons(port));

  if( (peer_socket == 0) || (peer_socket == -1) ){
    printf("Peer Connection failed\n");
    exit(0);
  }

  uint32_t photo_add;
  char image_name[MESSAGE_LEN];//Nome do ficheiro a enviar
  char input [MESSAGE_LEN];
  char command;


  printf("Choose one of the following commands\nAdd photo - a\n");
  printf("Exit program - q\n");
  printf("Command:");


  while(end==1){
    //Getting input command
    fgets(input, MESSAGE_LEN, stdin);
    sscanf(input,"%c",&command);

    switch(command){
    //Add-Photo
    case 'a' :
      printf("Input the name of the image you want to send\nImage name: ");
      fgets(input, MESSAGE_LEN, stdin);
      sscanf(input,"%s",image_name);
      photo_add = gallery_add_photo(peer_socket, image_name);
      printf("Picture ID: %d\n\n",photo_add);
      printf("Choose one of the following commands\nAdd photo - a\nCommand: ");
      break;
    //Quit Program
    case 'q':
      printf("Exiting the program\n");
      p.message_type = -99;
      printf("Message Type: %d\n",p.message_type);
      char *buff =(char *) malloc(sizeof (p));
      memcpy(buff, &p, sizeof(p));
      send(peer_socket, buff, sizeof(p),0);
      close(peer_socket);
      free(buff);
      exit(0);
      break;
    }
  }

  printf("END : %d\n", end);
  //Sending message to peer telling the client is disconnecting
  p.message_type = -99;
  printf("Message Type: %d\n",p.message_type);
  char *buff =(char *) malloc(sizeof (p));
  memcpy(buff, &p, sizeof(p));
  int nbytes = send(peer_socket, buff, sizeof(p),0);
  if(nbytes == -1){
    perror("Sending");
  }
  close(peer_socket);
  free(buff);
  free(act);
  exit(0);

}//end of main
static void handle(int sig, siginfo_t *siginfo,void *context){
  end=0;
}

#include "message.h"
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include "list.h"

//Global variables

peerlist * head;    //Head pointer to list
int sock_fd_peer;   //Peer communication socket
int sock_fd_client; //Client communication socket
void *cli_com();    //Code that the thread that comunicates with the client will run
void *peer_com();   //Code that the thread that comunicates with the peers will run

//Code that will be run when the Ctrl+C signal is recived(safe exit)
static void handle(int sig, siginfo_t *siginfo,void *context);

int main(){
  struct  sigaction act;
  pthread_t thread_c, thread_p;
  int iret_c, iret_p;

  //Sigaction Inicialazation
  memset (&act, '\0', sizeof(act));
  act.sa_sigaction = & handle;
  act.sa_flags = SA_SIGINFO;

  //DEfining Ctrl+C as a save closing method
  if(sigaction(SIGINT, &act, NULL) <0){
    perror("Sigaction:");
  }

  //Creation of the thread that will comunicate with the peer
  iret_p = pthread_create(&thread_p, NULL, peer_com, NULL);
  if(iret_p){
    perror("Peer thread:");
    exit(-1);
  }

  //Creation of the thread that will comunicate with the client
  iret_c = pthread_create(&thread_c, NULL, cli_com, NULL);
  if(iret_c){
    perror("Client thread:");
    exit(-1);
  }

  //Peer List Inicialazation
  head=InitList();

  while(cicle = 1);//Cicle so the main doesn't end and the threads keep running

  return 0;
}//end of main!

void *cli_com(){
  //É melhor dar nomes diferentes para não dar merda
  struct sockaddr_in local_addr; //Gateway's adress
  //struct sockaddr_in client_addr; //Client Adress

  //Creating Datagram socket that will comunicate with the client
  sock_fd_client = socket (AF_INET, SOCK_DGRAM, 0);
  if(sock_fd_client==-1){
    perror("Client Socket");
    exit(-1);
  }

  //Local Adress Inicialazation
  local_addr.sin_family = AF_INET;
  local_addr.sin_port=htons(3001);     //Port that comunicates with the client
  local_addr.sin_addr.s_addr=INADDR_ANY;

  int err = bind(sock_fd_client,(struct sockaddr *)&local_addr, sizeof(local_addr));
  if (err==-1){
    perror("Binding:");
    exit(-1);
  }

  sleep(10);

  PrintList(head);

  return 0;

}
void *peer_com(){

  socklen_t size_addr;
  int nbytes;

  message m ;
  char * buff;
  struct sockaddr_in local_addr; //Gateway's adress
  struct sockaddr_in peer_addr; //Peer Adress

  //Creating Datagram Socket that will comunicate with the server
  sock_fd_peer =socket(AF_INET, SOCK_DGRAM, 0);
  if(sock_fd_peer == -1){
    perror("Peer Socket:");
    exit(-1);
  }

  //Local Adress Inicialazation
  local_addr.sin_family = AF_INET;
  local_addr.sin_port=htons(3002);   //Port that comunicates with the peers
  local_addr.sin_addr.s_addr=INADDR_ANY;

  //Binding Socket to the local_addr
  int err = bind(sock_fd_peer,(struct sockaddr *)&local_addr, sizeof(local_addr));
  if (err==-1){
    perror("Binding:");
    exit(-1);
  }

  printf("Datagram socket created and binded\n");

  //Cicle that runs the gateway's Peer UPD connection
  while(1){

    size_addr = sizeof(peer_addr);

    buff =  (char*)malloc(sizeof (m));
    nbytes = recv(sock_fd_peer, buff ,sizeof (message), 0);

    /*Copying the memory of the recived buffer to the location of the
      memory location of the structure m*/
    memcpy(&m, buff, sizeof(message));
    free(buff);

    printf("Recived:\n");
    printf("%d \n", m.message_type);
    printf("%s \n", m.addr);
    printf("%d \n\n", m.port);
    //inet_aton
    printf("ip: %s\n", inet_ntoa(peer_addr.sin_addr));
    strcpy(m.addr,inet_ntoa(peer_addr.sin_addr));



    //insere na lista
    head=NewPeer(head, m.addr, m.port);
    PrintList(head);

    }

}

static void handle(int sig, siginfo_t *siginfo,void *context){
  printf("\nClosing Gateway, bye bye :(\n");
  close(sock_fd_peer);
  close(sock_fd_client);
  exit(0);
}

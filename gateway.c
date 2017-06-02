#include "message.h"
#include "list.h"
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

//Initialization of the mutex synco
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Global variables
peerlist * head;    //Head pointer to list
int sock_fd_peer;   //Peer communication socket
int sock_fd_client; //Client communication socket
int client_port=0;
int peer_port=0;
int end;        //Variable that decides the end of the program
void *cli_com();    //Code that the thread that comunicates with the client will run
void *peer_com();   //Code that the thread that comunicates with the peers will run

//Code that will be run when the Ctrl+C signal is recived(safe exit)
static void handle(int sig, siginfo_t *siginfo,void *context);

void Usage();

int main(int argc, char *argv[]){

  if(argc != 3){
    Usage();
    exit(0);
  }

  sscanf(argv[1], "%d", &client_port);
  sscanf(argv[2], "%d", &peer_port);

  if(client_port==0||peer_port==0){
    Usage();
    exit(0);
  }

  end=1;
  struct  sigaction *act;
  pthread_t thread_c, thread_p;
  int iret_c, iret_p;

  //Sigaction Initialization
  act = malloc(sizeof(act));
  act->sa_sigaction = &handle;
  act->sa_flags = SA_SIGINFO;
  if(sigaction(SIGINT, act, NULL) <0){
    perror("Sigaction:");
  }
  //Peer List Initialization
  head=InitList();

  printf("Gateway up and running\n");

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


  while(end == 1);

  PrintList(head);
  FreeList(head);

  close(sock_fd_client);
  close(sock_fd_peer);
  printf("\nClosing Gateway\n");
  printf("Thank you for your visit.\n");
  free(act);
  exit (0);
}//end of main!



void *cli_com(){
  socklen_t size_addr;
  int nbytes;                    //Number of bytes recived or sent
  message m ;                    //Struture that will handle the messages
  char *buff;

  struct sockaddr_in local_addr; //Gateway's adress
  struct sockaddr_in client_addr; //Client Adress

  //Creating Datagram socket that will comunicate with the client
  sock_fd_client = socket (AF_INET, SOCK_DGRAM, 0);
  if(sock_fd_client==-1){
    perror("Client Socket");
    exit(-1);
  }

  //Local Adress Inicialazation
  local_addr.sin_family = AF_INET;
  local_addr.sin_port=htons(client_port);     //Port that comunicates with the client
  local_addr.sin_addr.s_addr=INADDR_ANY;

  int err = bind(sock_fd_client,(struct sockaddr *)&local_addr, sizeof(local_addr));
  if (err==-1){
    perror("Binding:");
    exit(-1);
  }

  size_addr = sizeof(client_addr);

  while(1){

    buff =  (char*)malloc(sizeof (m));
    nbytes = recvfrom(sock_fd_client, buff ,sizeof (message), 0,
                      (struct sockaddr *) &client_addr, &size_addr);
    if(nbytes == -1){
      perror("Reciving");
      exit(-1);
    }

    memcpy(&m, buff, sizeof(message));


    printf("\nNew client\n");

    //If the client wants to know the data of a peer to connect, message_type = 0
    if(m.message_type == 0){

      //Locking thread accessibility to the list
      pthread_mutex_lock(&mutex);
      if(CountPeers(head)>0){
        //Passing the server data stored in the list to a structure of type message
        strcpy(m.addr, GiveIP(head));
        m.port = GivePort(head);
        head = NextPeer(head);

        printf("Sent to client:\n");
        printf("%s \n", m.addr);
        printf("%d \n\n", m.port);
      }else{
        m.message_type = -1;
      }

      memcpy(buff, &m, sizeof(m));
      nbytes = sendto(sock_fd_client, buff, sizeof(m), 0,
                      (struct sockaddr *) &client_addr, size_addr);

      if(nbytes == -1){
        perror("Sending");
        exit(-1);
      }

      //Unlocking thread accessibility to the list
      pthread_mutex_unlock(&mutex);
    }
    free(buff);
  }
}//End of Client Communication thread

void *peer_com(){
  int nbytes;                    //Nuber of bytes recived or sent
  message m ;                    //Struture that will handle the messages
  char * buff;
  int npeers;

  socklen_t size_addr;

  struct sockaddr_in local_addr; //Gateway's adress
  struct sockaddr_in peer_addr; //Client Adress

  //Creating Datagram Socket that will comunicate with the server
  sock_fd_peer =socket(AF_INET, SOCK_DGRAM, 0);
  if(sock_fd_peer == -1){
    perror("Peer Socket:");
    exit(-1);
  }

  //Local Adress Inicialazation
  local_addr.sin_family = AF_INET;
  local_addr.sin_port=htons(peer_port);   //Port that comunicates with the peers
  local_addr.sin_addr.s_addr=INADDR_ANY;

  //Binding Socket to the local_addr
  int err = bind(sock_fd_peer,(struct sockaddr *)&local_addr, sizeof(local_addr));
  if (err==-1){
    perror("Binding:");
    exit(-1);
  }

  size_addr = sizeof(peer_addr);
  //Cicle that runs the gateway's Peer UPD connection
  while(1){

    buff =  (char*)malloc(sizeof (m));
    nbytes = recvfrom(sock_fd_peer, buff ,sizeof (message), 0,
                      (struct sockaddr *) &peer_addr, &size_addr);
    if(nbytes == -1){
      perror("Reciving");
      exit(-1);
    }

    /*Copying the memory of the recived buffer to the location of the
      memory location of the structure m*/
    memcpy(&m, buff, sizeof(message));
    free(buff);

    if(m.message_type == 1){

      printf("\nNew Peer:\n");
      printf("%s \n", inet_ntoa(peer_addr.sin_addr));
      printf("%d \n\n", m.port);

//Sending amount of currently connected Peers
      npeers = CountPeers(head);
      nbytes = sendto(sock_fd_peer, &npeers, sizeof(npeers), 0,
                      (struct sockaddr *) &peer_addr, size_addr);
      if(nbytes == -1){
        perror("Sending");
      }
      if(npeers != 0){
        //Sending byte array with the peer list information
        peerlist list[npeers];
        peerlist *aux = head;
        printf("Nº de peers: %d\n", npeers);
        for(int i=0; i<npeers; i++){
          list[i]=*aux;
          pthread_mutex_lock(&mutex);
          aux=NextPeer(aux);
          pthread_mutex_unlock(&mutex);
        }

        buff = (char *) malloc(sizeof(peerlist)*npeers);
        memcpy(buff, list, sizeof(peerlist)*npeers);

        nbytes = sendto(sock_fd_peer, buff, sizeof(peerlist)*npeers, 0,
                              (struct sockaddr *) &peer_addr, size_addr);
        if(nbytes == -1){
          perror("Sending");
        }
    }
      //Send message to all other peers?
      pthread_mutex_lock(&mutex);
      //Insertion of the Peer in the peer list
      head=NewPeer(head, m.addr, m.port);
      pthread_mutex_unlock(&mutex);
    }
    if(m.message_type == -1){
      printf("Remove Peer:\n");
      printf("%s \n", m.addr);
      printf("%d \n\n", m.port);

      pthread_mutex_lock(&mutex);
      head = RemovePeer(head, m.addr, m.port);
      PrintList(head);
      pthread_mutex_unlock(&mutex);
    }
  }
}//End of peer communication

void Usage(){
  printf("Usage: ./gateway client_port peer_port \n");
  return;
}

//Sigaction handler, aka, interruption that will kill the program safelly by pressing Ctrl+C
static void handle(int sig, siginfo_t *siginfo,void *context){
  end=0;
}

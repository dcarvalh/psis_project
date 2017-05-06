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


//Global variables

peerlist * head;    //Head pointer to list
int sock_fd_peer;   //Peer communication socket
int sock_fd_client; //Client communication socket
int end = 1;        //Variable that decides teh end of the program

void *cli_com();    //Code that the thread that comunicates with the client will run
void *peer_com();   //Code that the thread that comunicates with the peers will run
//Code that will be run when the Ctrl+C signal is recived(safe exit)
static void handle(int sig, siginfo_t *siginfo,void *context);

int main(){
  struct  sigaction act;
  pthread_t thread_c, thread_p;
  int iret_c, iret_p;

  //Sigaction Initialization
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

  //Peer List Initialization
  head=InitList();

  while(end);//Cicle so the main doesn't end and the threads keep running

  close(sock_fd_client);
  close(sock_fd_peer);
  printf("\nClosing Gateway\n");
  printf("Thank you for your visit\n");
  exit (0);
}//end of main!

void *cli_com(){
  //É melhor dar nome diferentes para nao dar merda(nunca vai dar porque o para dar tinah que ser defenido globalmente)
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
  local_addr.sin_port=htons(3001);     //Port that comunicates with the client
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
  free(buff);

  printf("\nmessage_type: %d\n", m.message_type);

///////////////////////////////////////////////////////////////////////////////////////////////////////
    //If the client wants to know the data of a peer to connect, message_type = 0
    if(m.message_type == 0){

      //Passing the server data stored in the list to a structure of type message
      m = FillMessage(head);

      buff =(char *) malloc(sizeof (m));                                       // VE ESTA PARTE!A PARTE DO RECVFROM TA TODA OF E A DO CLIENT TAMBEM
      memcpy(buff, &m, sizeof(m));                                             //O PROBLEMA ERA A ENVIAR, QUE NAO CONSEGUI PASSAR AS CENAS DO HEAD
                                                                               //DIRECTAMENTE PARA O M.ADDR E PARA O M.PORT, QUE DIZIA QUE LISTPEER ESTAVA UNDEFINED
      //Sending message to gateway
      nbytes = sendto(sock_fd_client, buff, sizeof(m), 0,
                  	  (struct sockaddr *) &client_addr, size_addr);
      if(nbytes == -1){
        perror("Sending");
        exit(-1);
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////
    }
  }
}//End of Client Communication
void *peer_com(){
  int nbytes;                    //Nuber of bytes recived or sent
  message m ;                    //Struture that will handle the messages
  char * buff;
  struct sockaddr_in local_addr; //Gateway's adress

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

    buff =  (char*)malloc(sizeof (m));
    nbytes = recv(sock_fd_peer, buff ,sizeof (message), 0);
    if(nbytes == -1){
      perror("Reciving");
      exit(-1);
    }

    /*Copying the memory of the recived buffer to the location of the
      memory location of the structure m*/
    memcpy(&m, buff, sizeof(message));
    free(buff);

    if(m.message_type == 1){

      //Insertion of the Peer in the peer list
      head=NewPeer(head, m.addr, m.port);
    }
  }
}//End of peer communication
//Sigaction handler, aka, interruption that will kill the program safelly by pressing Ctrl+C
static void handle(int sig, siginfo_t *siginfo,void *context){
  end = 0;
}

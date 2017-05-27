#include "message.h"
#include "img_list.h"

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
#include <time.h>

#define MAX_WAIT_LIST 5

/*Used sockets in th server must be declared as global variables to be closed in
  the sigaction function*/

int new_cli_sock;
int sock_TCP;
int sock_gateway_fd;

struct sockaddr_in gateway_addr;
char *buff;
message m;
struct sockaddr_in client_addr;

int client_count=0;
struct sigaction *act;

//Picture list variables
photolist *head;

void *cli_com(void  *new_cli_sock);
static void handle(int sig, siginfo_t *siginfo,void *context);


int main (){
  //Structures that will save the information of the several
  struct sockaddr_in local_addr;
  socklen_t addrlen;
  int nbytes;


  //Threads that will comunicate with the client
  pthread_t thread_c;
  int iret_c;
  int *t_args;



////Sigaction Initialization
  act = malloc(sizeof(act));
  act->sa_sigaction = & handle;
  act->sa_flags = SA_SIGINFO;
//Defining Ctrl+C as a save closing method
  if(sigaction(SIGINT, act, NULL) <0){
    perror("Sigaction:");
  }

//Socket that comunicates with Gateway
  sock_gateway_fd = socket(AF_INET, SOCK_DGRAM,0);
  if(sock_gateway_fd==-1){
    perror("Socket:");
    exit(-1);
  }

  //Incialização da local address
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(3000+getpid());
  local_addr.sin_addr.s_addr=INADDR_ANY;

  //Incialização da gatewa address
  gateway_addr.sin_family = AF_INET;
  gateway_addr.sin_port = htons(3002);
  gateway_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

  printf("Datagram socket created and binded\n");

//Filling uo the structure to sent the Gateway
  m.port=ntohs(local_addr.sin_port);
  m.message_type= 1;
  strcpy(m.addr, "127.0.0.1");

  printf("\nSent:\n");
  printf("%d \n", m.message_type);
  printf("%s \n", m.addr);
  printf("%d \n\n", m.port);

//copying the memory of the structure to the mem location of a buffer
  buff =(char *) malloc(sizeof (m));
  memcpy(buff, &m, sizeof(m));

//Sending conect message to gateway_addr
  nbytes = sendto(sock_gateway_fd, buff, sizeof(m), 0,
                	  (const struct sockaddr *) &gateway_addr,sizeof(gateway_addr));

  printf("bytes sent: %d \n", nbytes);

  /////////////////Client handeling portion//////////////////////

  //Binding and creating TCP socket

  sock_TCP = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_TCP == -1){
    perror("Creating socket \n ");
    exit(-1);
  }
  int yes=1;
  if (setsockopt(sock_TCP, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("Setsockopt error");
		exit(-1);
	}
  int err = bind(sock_TCP,(struct sockaddr *)&local_addr, sizeof(local_addr));
  if (err==-1){
    perror("Binding:");
    exit(-1);
  }

  t_args = malloc(sizeof(new_cli_sock));

  //Initializing Photo list
  head = InitPhotoList();

  while(1){
    /*Listening for new clients connecting(in case more than one connects at the
                                            same time, creates a queue)*/
    listen(sock_TCP, MAX_WAIT_LIST);

    //Accepting and creating new socket for the client
    new_cli_sock= accept(sock_TCP, (struct sockaddr *) & client_addr, &addrlen);
    client_count++;
    printf("Client Acepted!\nClient Count:%d\n\n", client_count);

    t_args= &new_cli_sock;
    //Creation of the thread that will comunicate with the client
    iret_c = pthread_create(&thread_c, NULL, cli_com, t_args);
    if(iret_c){
      perror("Client thread:");
      exit(-1);
    }
  }

}//END OF MAIN

void *cli_com(void *new_cli_sock){

  //Socket communication variables
  int fd = *(int*)new_cli_sock;
  char *buff;
  socklen_t  size_addr = sizeof(client_addr);
  //Picture variables
  pic_info pi;
  uint32_t pic_id;

  while(1){

    //Reciving message from client
    buff =  (char*)malloc(sizeof (pi));
    int nbytes = recv(fd, buff ,sizeof (pi), 0);
    if(nbytes==-1){
      perror("Reciving");
      sleep(5);
    }
    memcpy(&pi, buff, sizeof(pi));
    free(buff);


    switch (pi.message_type){
      //Exiting protocol
      case -99:
      {
        int retval = 1;
        client_count--;
        printf("A client has left.\nClient Count: %d\n\n", client_count);
        pthread_exit ((void*) &retval);
      break;
      }
      ///////////////////
      //Add_picture protocol
      case 2:
      {
        char server_img[1000];
        char p_array[pi.size];
        FILE *image;
        printf("Picture Size: %d\nPicture Name: %s\n", pi.size, pi.pic_name );
        clock_t current = clock();
        pic_id = getpid() + (current*10000);
        sprintf(server_img, "%d", pic_id);
        //Recive byte image array
        printf("Reading Picture Byte Array\n");
        read(fd, p_array, pi.size);
        //Adding photo to the photo lista
        head = NewPhoto(head, pic_id, pi.pic_name);
        //Reconstruct byte array into picture
        printf("Converting Byte Array to Picture\n");
        image = fopen( server_img, "w");
        fwrite(p_array, 1, sizeof(p_array), image);
        fclose(image);
        nbytes = sendto(fd, &pic_id, sizeof(pic_id), 0,
                        (struct sockaddr *) &client_addr, size_addr);
        if(nbytes == -1){
          perror("Sending");
          exit(-1);
        }
        printf("Picture Added!\n\n");
        PrintPhotoList(head);
      break;
    }
      ////////////////End ADD PICTURE!
      /////////////Add Keyword protocol//////////////////
      case 3:
      {
        photolist *aux = head;
        keyword  *k_head;
        if((aux = GetPhoto(head, pi.size))!=NULL){
          k_head = GetKeyHead(aux);
          k_head = NewKeyWord(k_head, pi.pic_name);
          Adding(aux, k_head);
          PrintKeyWords(aux);
          pi.message_type = 1;
          printf("Keyword added!\n");
        }else{
          pi.message_type = -1;
        }
        buff =  (char*)malloc(sizeof (pi));
        memcpy(&pi, buff, sizeof(pi));
        nbytes = sendto(fd, buff, sizeof(pi), 0,
                        (struct sockaddr *) &client_addr, size_addr);
        if(nbytes == -1){
          perror("Sending");
          exit(-1);
        }
        free(buff);
      break;
    }

      ////////////////////END ADD KEYWORD
      //// SEARCH PHOTO protocol
      case 4:
      {
        uint32_t photos[50];

        int n = SearchPhotosbyKeyWords(head, pi.pic_name, photos);

        printf("Nº Photos: %d\n", n);
        //Sending amount of photos found
        nbytes = sendto(fd, &n, sizeof(n), 0,
                        (struct sockaddr *) &client_addr, size_addr);
        for(int i=0; photos[i] != 0; i++)
          printf("%" PRIu32 "\n", photos[i]);
        //Sending array as a byte stream
        nbytes = sendto(fd, &photos, n*sizeof(uint32_t), 0,
                        (struct sockaddr *) &client_addr, size_addr);
        if(nbytes == -1){
          perror("Sending");
          exit(-1);
        }
        break;

      }
      default:
      {
          printf("Invalid Message Type recived\n");
          sleep(5);
      }
    }
  }
}//END OF CLIENT THREAD

static void handle(int sig, siginfo_t *siginfo,void *context){

  m.message_type = -1;//message of the type -1 tells the gateway the server is disconnecting

  PrintPhotoList(head);

  buff =(char *) malloc(sizeof (m));
  memcpy(buff, &m, sizeof(m));

//Sending disconect message to gateway_addr
  sendto(sock_gateway_fd, buff, sizeof(m), 0,
                	  (const struct sockaddr *) &gateway_addr,sizeof(gateway_addr));
  close(new_cli_sock);
  close(sock_TCP);
  close(sock_gateway_fd);
  FreePhotoList(head);
  PrintPhotoList(head);
  free(buff);
  free(act);
  exit(0);
}

#include "message.h"
#include "img_list.h"
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
#include <time.h>
#define MAX_WAIT_LIST 5

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// GLOBAL VARIABLES ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*Used sockets in the server must be declared as global variables to be closed in
  the sigaction function*/
int npeers=0;
peerlist *genlist;  //Vector that will keeps the existing peers

int new_cli_sock;        //fd for new clients
int sock_TCP;            //fd for accepting new clients
int sock_gateway_fd;     //fd for communicating with gateway


struct sockaddr_in gateway_addr; //for communicating with gateway
message m;                       //struct for communicating with gateway

struct sockaddr_in server_addr;

int client_count = 0;   //number of active clients
struct sigaction *act;  //sigaction handler

photolist *head;  //Picture list head


int nbytes; //number of bytes read or sent

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// HEADERS //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *cli_com(void  *new_cli_sock);
static void handle(int sig, siginfo_t *siginfo,void *context);

void Add_picture(int fd, pic_info pi);
void Add_keyword(int fd, pic_info pi);
void Search_picture(int fd, pic_info pi);
void Delete_picture(int fd, pic_info pi);
void Picture_name(int fd, pic_info pi);
void Get_picture(int fd, pic_info pi);

peerlist *peer_head; //Head to the list of all other peers
void Broadcast(int messagetype, peerlist *peerlist, int npeers);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// MAIN ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int main (){
  //Structures that will save the information of the several

  struct sockaddr_in client_addr;

  struct sockaddr_in local_addr;

  socklen_t addrlen;

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

  //Incialização gateway address
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
  char *buff;
  buff =(char *) malloc(sizeof (m));
  memcpy(buff, &m, sizeof(m));

//Sending conect message to gateway_addr
  nbytes = sendto(sock_gateway_fd, buff, sizeof(m), 0,
                	  (const struct sockaddr *) &gateway_addr,sizeof(gateway_addr));

  printf("bytes sent: %d \n", nbytes);

  //////////////////////////////////Replication///////////////////////////////////////////////////////
  //Recive number of current existing peers

  nbytes=recv(sock_gateway_fd, &npeers, sizeof(npeers),0);
  if(nbytes == -1){
    perror("Reciving");
    exit(-1);
  }
  peer_head = InitList();

  peerlist list[npeers];
  genlist = list;
  if(npeers != 0){
    buff = (char *) malloc(sizeof(peerlist)*npeers);
    nbytes = recv(sock_gateway_fd, buff, sizeof(peerlist)*npeers,0);
    if(nbytes == -1){
      perror("Reciving");
      exit(-1);
    }
    memcpy(list, buff, sizeof(peerlist)*npeers);
    //Broadcasting new peer existence
    Broadcast(19, genlist, npeers);

    //Initializing photo list
    head = InitPhotoList();
    // Getting photos and keywords from a peer
    pic_info broad;
    broad.message_type = 2000; //Message type for a new server
    server_addr.sin_family = AF_INET;
    int sock_fd_server= socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd_server == -1){
      perror("Socket\n");
      exit(-1);
    }
    server_addr.sin_port= htons(GivePort(peer_head));
    inet_aton(GiveIP(peer_head), &server_addr.sin_addr);
    //Connecting to peer to get photos
    if(connect(sock_fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
      perror("Connecting");
      exit(-1);
    }
    buff = (char *) malloc(sizeof(pic_info));
    memcpy(buff, &broad, sizeof(pic_info));
    nbytes = send(sock_fd_server, buff, sizeof(broad), 0);
    if(nbytes==-1){
      perror("Sending");
    }
    //Revice size of photo list
    int photosize=0;
    nbytes = recv(sock_fd_server, &photosize, sizeof(photosize), 0);
    if(nbytes == -1){
      perror("Reciving");
      exit(-1);
    }
    printf("PhotoSize: %d\n", photosize);
    //Receber lista de photos

    if(photosize != 0){

      int w= 0;

      buff = (char *) malloc(sizeof(photolist)*photosize);
      nbytes = recv(sock_fd_server, buff, sizeof(photolist)*photosize, 0);
      if(nbytes == -1){
        perror("Reciving");
        exit(-1);
      }
      photolist testlist[photosize];
      memcpy(testlist, buff, sizeof(photolist)*photosize);

      printf("id_photo: %u\n", testlist[0].id_photo);
      printf("file_name: %s\n", testlist[0].file_name);

      photolist *aux;
      aux = head;
      for(int i=0; i<photosize; i++){
        printf("Inserting photos\n");
        head = InsertPhotoEnd(head, testlist[i].id_photo, testlist[i].file_name);
      }

      printf("Photos Inserted\n");
      int keywordsize=0;
      keyword *keywordvector;
      BICHO;
      for(aux = head; aux!=NULL; aux=aux->next){
        nbytes = recv(sock_fd_server, &keywordsize, sizeof(keywordsize), 0);
        if(nbytes == -1){
          perror("Reciving");
        }
        printf("Keywordsize: %d\n", keywordsize);
        if(keywordsize == 0)
          break;

        //Getting new keyword to add to photo
        keywordvector = malloc(sizeof(keyword)*keywordsize);
        buff = (char *) malloc(sizeof(keyword)*keywordsize);
        recv(sock_fd_server, buff, sizeof(keyword)*keywordsize, 0);
        memcpy(keywordvector, buff, sizeof(keyword)*keywordsize);
        printf("Keyword: %s\n",keywordvector[w].keyword_name);
        w++;
        for(int i=0; i<keywordsize; i++){
          NewKeyWord(aux, keywordvector[i].keyword_name);
        }
      }

      //Reciving pictures
      for(int i =0; i<photosize; i++){
        uint32_t id;
        recv(sock_fd_server, &id, sizeof(id), 0);
        //Receiving size of photo
        long pic_size;
        nbytes=recv(sock_fd_server, &pic_size, sizeof(pic_size), 0);
        if(nbytes==-1){
          perror("Receiving");
          exit(-1);
        }
        char name[20];
        sprintf(name,"%u", id);
        //Reciving byte array
        FILE *picture;
        char p_array[pic_size];
        recv(sock_fd_server, p_array, pic_size, 0);
        //Reconstructing byte array
        picture = fopen(name, "w");
        fwrite(p_array, 1, sizeof(p_array), picture);
        fclose(picture);
      }
    }

    close(sock_fd_server);
    }else{
      printf("No other peers exist at the moment\n");
      //Initializing photo list
      head = InitPhotoList();
    }
  /////////////////Client handeling portion//////////////////////

  PrintPhotoList(head);
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

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// CLIENT THREAD ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void *cli_com(void *new_cli_sock){

  //Socket communication variables
  int fd = *(int*)new_cli_sock;
  char *buff;
  //Picture variables
  pic_info pi;

  while(1){
    //Reciving message from client
    pi.size=0; //inicializar o ID
    buff =  (char*)malloc(sizeof (pi));
    bzero(buff, sizeof(pi));
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

      case 2:
      {
        Add_picture(fd, pi);
        PrintPhotoList(head);
        break;
      }
      case 3:
      {
        Add_keyword(fd, pi);
        PrintPhotoList(head);
        break;
      }
      case 4:
      {
        Search_picture(fd, pi);
        break;
      }
      case 5:
      {
        Delete_picture(fd, pi);
        PrintPhotoList(head);
        break;
      }
      case 6:
      {
        Picture_name(fd, pi);
        break;
      }
      case 7:
      {
        Get_picture(fd, pi);
        break;
      }

      /////////////////////////////END OF GET PHOTO PROOCOL
      //NEw peer connected protocol
      case 19:
      {
        peer_head = NewPeer(peer_head, pi.pic_name, pi.size);
        PrintList(peer_head);
        client_count--;
        pthread_exit (NULL);
        break;
      }
      //Peer disconnected
      case -19:
      {
        peer_head = RemovePeer(peer_head, pi.pic_name, pi.size);
        client_count--;
        PrintList(peer_head);
        pthread_exit (NULL);
      }

      ///Enviar lista de fotos
      case 2000:
      {
        client_count--;
        photolist *aux;
        int photocount = 0;

        for(aux = head; aux != NULL; aux = aux->next){
            photocount++;
        }
        int nbytes = send(fd, &photocount, sizeof(photocount), 0);
        if(nbytes==-1){
          perror("Sending");
        }

        //Filling photo list vector with the photo data
        aux = head;
        photolist p_list[photocount];
        for(int i = 0; i<photocount; i++){
          p_list[i]=*aux;
          p_list[i].key_head = NULL;
          aux = aux->next;
        }

        //Sending list of photos to the new peer
        buff = malloc(sizeof(photolist)*photocount);
        memcpy(buff, p_list, sizeof(photolist)*photocount);

        nbytes = send(fd, buff, sizeof(photolist)*photocount, 0);
        if(nbytes==-1){
          perror("Sending");
        }


        //Filling keyword vector with photo keyword
        keyword *k;
        int keycount;
        keyword *k_vector;
        for(aux = head; aux != NULL; aux=aux->next){
          keycount = 0;
          for(k=aux->key_head; k!=NULL; k=k->next_key)
            keycount++;
          printf("Keycount: %d\n", keycount);
          //Breaking if no keywords exist
          nbytes = send(fd, &keycount, sizeof(keycount), 0);
          if(nbytes==-1){
            perror("Sending");
          }
          BICHO;
          if(keycount == 0)
            break;
          k_vector = malloc(sizeof(keyword)*keycount);
          k = aux->key_head;
          for(int i=0; i<keycount; i++){
            k_vector[i]=*k;
            k = k->next_key;
            printf("Keywords: %s\n",k_vector[i].keyword_name);
          }
          //Sending photo keyword list to new peer
          buff = (char *) malloc(sizeof(keyword)*keycount);
          memcpy(buff, k_vector, sizeof(photolist)*photocount);

          nbytes = send(fd, buff, sizeof(keyword)*keycount, 0);
          if(nbytes==-1){
            perror("Sending");
          }
        }
        if(keycount != 0)
          free(k_vector);

        for(aux = head; aux!=NULL; aux= aux->next){

          FILE *picture;
          long pic_size;
          //Reading stored picture
          char name[15];
          sprintf(name, "%"PRIu32, aux->id_photo);
          picture=fopen(name, "rb");
          if(picture == NULL){
            perror("Filename");
            pic_size = -1;
          }
          nbytes = send(fd, &aux->id_photo, sizeof(uint32_t), 0);
          //Searching the beggining and end of the picture
          fseek(picture, 0, SEEK_END);
          pic_size = ftell(picture);
          rewind(picture);
          //Sending picture size to client_addr
          nbytes = send(fd, &pic_size, sizeof(pic_size), 0);
          if(nbytes == -1){
            perror("Sending");
            exit(-1);
          }
          //Sending Picture as byte array
          char send_buffer[pic_size];
          size_t fr;
          while(!feof(picture)){  //Reading file, while it is not the end of file
            fr=fread(send_buffer ,sizeof(char), sizeof(send_buffer), picture);
            if(fr>0){
              nbytes = send(fd, send_buffer, sizeof(send_buffer), 0);
              if(nbytes == -1){
                perror("Sending:");
                exit(0);
              }
            }
            bzero(send_buffer, sizeof(send_buffer));
          }
          fclose(picture);
        }//END FOR
        pthread_exit(NULL);
        break;
      }

      default:
      {
          printf("Invalid Message Type recived\n");
          sleep(5);
          break;
      }
    }

  }
}//END OF CLIENT THREAD

///////////////////////////////// CASES ////////////////////////////////////////

void Add_picture(int fd, pic_info pi){
  uint32_t pic_id;
  pic_id = 0;
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
  nbytes = send(fd, &pic_id, sizeof(pic_id), 0);

  if(nbytes == -1){
    perror("Sending");
    exit(-1);
  }
  printf("Picture Added!\n\n");

  return;
}

void Add_keyword(int fd, pic_info pi){
  uint32_t photos[50];
  int k=0;
  photolist *aux = head;
  if((aux = GetPhoto(head, pi.size))!=NULL){
    SearchPhotosbyKeyWords(head, pi.pic_name, photos);
    for(int i=0; photos[i] != 0; i++){
      if(photos[i]==GetID(aux)){
        k = -1; //keyword already exists in that photo
      }
    }
    if(k!=-1){
      NewKeyWord(aux, pi.pic_name);
      k = 1;
    }
  }else{
    k = -1;
  }

  nbytes = send(fd, &k, sizeof(k), 0);
  if(nbytes == -1){
    perror("Sending");
    exit(-1);
  }

  return;
}
/*
int i;
for(i=0; i<50; i++){
  photos[i]=0;        Get_picture(fd, pi);
      break;
    }
}*/

void Search_picture(int fd, pic_info pi){
  uint32_t photos[50];
  int i;
  for(i=0; i<50; i++){
    photos[i]=0;
  }
  int n = SearchPhotosbyKeyWords(head, pi.pic_name, photos);
  printf("Nº Photos with keyword '%s': %d\n", pi.pic_name, n);
  //Sending amount of photos found
  nbytes = send(fd, &n, sizeof(n), 0);
  for(i=0; photos[i] != 0; i++){
    if(i==0)
      printf("Photos: \n");
    printf("%" PRIu32 "\n", photos[i]);
  }
  printf("\n");
  //Sending array as a byte stream
  nbytes = send(fd, &photos, n*sizeof(uint32_t), 0);
  if(nbytes == -1){
    perror("Sending");
    exit(-1);
  }
}

void Delete_picture(int fd, pic_info pi){
  int k;
  photolist *aux;
  if((aux = GetPhoto(head, pi.size))!=NULL){
    printf("Delete photo " "%" PRIu32 "\n", pi.size);
    head=DeletePhoto(head, aux);
    if(head==(photolist * )-1){
      perror("deleting photo");
      exit(-1);
    }
    k = 1;
  }else{
    k = 0;
  }
  nbytes = send(fd, &k, sizeof(k), 0);
  if(nbytes == -1){
    perror("Sending");
    exit(-1);
  }

  return;
}

void Picture_name(int fd, pic_info pi){
  pic_info p;
  char * photo_name;
  photolist *aux;
  if((aux = GetPhoto(head, pi.size))!=NULL){
    photo_name=GetPhotoName(aux);
    strcpy(p.pic_name,photo_name);
    p.message_type = 1;
  }else{
    p.message_type  = 0;
  }
  char *buff;
  buff =(char *) malloc(sizeof (p));
  memcpy(buff, &p, sizeof(p));

  nbytes = send(fd, buff, sizeof(p), 0);
  if(nbytes == -1){
    perror("Sending");
    exit(-1);
  }

  return;
}



void Get_picture(int fd, pic_info pi){
  photolist *photo;
  //Verifying if the requested picture exisits
  if((photo = GetPhoto(head, pi.size))!=NULL){
    FILE *picture;
    long pic_size;
    //Reading stored picture
    char name[15];
    sprintf(name, "%"PRIu32, pi.size);
    picture=fopen(name, "rb");
    if(picture == NULL){
      perror("Filename");
      pic_size = -1;
    }
    //Searching the beggining and end of the picture
    fseek(picture, 0, SEEK_END);
    if(pic_size != -1){
      pic_size = ftell(picture);
    }
    rewind(picture);
    //Sending picture size to client_addr
    int nbytes = send(fd, &pic_size, sizeof(pic_size), 0);
    if(nbytes == -1){
      perror("Sending");
      exit(-1);
    }
    //Sending Picture as byte array
    char send_buffer[pic_size];
    size_t fr;
    while(!feof(picture)){  //Reading file, while it is not the end of file
      fr=fread(send_buffer ,sizeof(char), sizeof(send_buffer), picture);
      if(fr>0){
        nbytes = send(fd, send_buffer, sizeof(send_buffer), 0);
        if(nbytes == -1){
          perror("Sending:");
          exit(0);
        }
      }
      bzero(send_buffer, sizeof(send_buffer));
    }
    fclose(picture);
  }else{
    //Returning to client that there is no photo with the requested ID
    int pic_size = 0;
    int nbytes = send(fd, &pic_size, sizeof(pic_size), 0);
    if(nbytes == -1){
      perror("Sending");
      exit(-1);
    }
  }

  return;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// HANDLER ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void handle(int sig, siginfo_t *siginfo,void *context){

  PrintPhotoList(head);
  char * buff;
  buff =(char *) malloc(sizeof (m));
  //Sending disconect message to gateway_addr
  m.message_type = -1;//message of the type -1 tells the gateway the server is disconnecting
  memcpy(buff, &m, sizeof(m));

  //Sending disconect message to gateway_addr
  sendto(sock_gateway_fd, buff, sizeof(m), 0,
                	  (const struct sockaddr *) &gateway_addr,sizeof(gateway_addr));

  Broadcast(-19, genlist, npeers);

  close(new_cli_sock);
  close(sock_TCP);
  close(sock_gateway_fd);
  FreePhotoList(head);
  //PrintPhotoList(head); //debug
  free(buff);
  free(act);
  exit(0);
}

void Broadcast(int messagetype, peerlist *peerlist, int npeers){

  char * buff;
  pic_info broad;
  broad.message_type = messagetype; //Message type for a new server
  strcpy(broad.pic_name, m.addr);
  broad.size = m.port;
  server_addr.sin_family = AF_INET;

  //Placing Peers in list and sending them new peer connection message
  for(int i=0; i<npeers; i++){
      int sock_fd_server= socket(AF_INET, SOCK_STREAM, 0);
      if(sock_fd_server == -1){
        perror("Socket\n");
        exit(-1);
      }
      peer_head = NewPeer(peer_head,genlist[i].ip,genlist[i].port);
      server_addr.sin_port= htons(GivePort(peer_head));
      inet_aton(GiveIP(peer_head), &server_addr.sin_addr);
      //Connecting to peer to anounce new peer existence
      if(connect(sock_fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("Connecting");
        exit(-1);
      }
      buff = (char *) malloc(sizeof(pic_info));
      memcpy(buff, &broad, sizeof(pic_info));
      int nbytes = send(sock_fd_server, buff, sizeof(broad), 0);
      if(nbytes==-1){
        perror("Sending");
      }
      close(sock_fd_server);
  }
}

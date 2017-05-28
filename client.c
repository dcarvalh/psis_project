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
#include <inttypes.h>

//Variables that are used in the signal handlelign function
int end =1;
//Signal handlelling function
static void handle(int sig, siginfo_t *siginfo,void *context);

int main (){

  char * host = "127.0.0.1";
  int port = 3001;
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



  printf("\nChoose one of the following commands\n");
  printf("\ta - Add photo\n");
  printf("\td - Delete photo\n");
  printf("\tk - Add a keyword to the picture \n");
  printf("\ts - Search for photos by keyword\n");
  printf("\tg - Download a photo\n");
  printf("\tq - Exit program \n");
  printf("Command: ");



  int k;
  int key_id;
  char *k_word;
  command ='\0';
  while(end==1){
    //Getting input command
    fgets(input, MESSAGE_LEN, stdin);
    sscanf(input,"%c",&command);
    printf("\n");
    switch(command){
    //Add Photo
    case 'a' :
    {
      printf("Input the name of the image you want to send\nImage name: ");
      fgets(input, MESSAGE_LEN, stdin);
      sscanf(input,"%s",image_name);
      photo_add = gallery_add_photo(peer_socket, image_name);

      printf("Picture ID: %d\n\n",photo_add);

      break;
    }
    //Quit Program
    case 'q':
    {
      printf("Exiting the program\n");
      p.message_type = -99;
      char *buff =(char *) malloc(sizeof (p));
      memcpy(buff, &p, sizeof(p));
      send(peer_socket, buff, sizeof(p),0);
      close(peer_socket);
      free(buff);
      exit(0);
      break;
    }
    //Add Keywords
    case 'k':
    {
      printf("Insert keyword you want to add: ");
      fgets(input, MESSAGE_LEN, stdin);
      k_word = malloc(sizeof(input)*sizeof(char));
      sscanf(input,"%s",k_word);
      printf("Insert the ID of the photo to add the keyword: ");
      fgets(input, MESSAGE_LEN, stdin);
      sscanf(input,"%d",&key_id);//  "%"PRIu32
      k = gallery_add_keyword(peer_socket, key_id, k_word);
      if(k == 1)
        printf("Keyword sucessefully Added!\n");
      else
        printf("Could not add keyword\nUnlucky\n");
      printf("\n");
      break;
    }
    //Delete photo
    case 'd':
    {
      printf("\nInsert the ID of the photo to delete: ");
      fgets(input, MESSAGE_LEN, stdin);
      sscanf(input,"%d",&key_id);//  "%"PRIu32

      k=gallery_delete_photo(peer_socket, key_id);

      if(k == 1)
        printf("Photo sucessefully deleted!\n");
      else if (k == 0)
        printf("Photo does not exist.\n");
      else
        printf("Error deleting the photo.\n");
      break;
    }
    //Search photo
    case 's':
    {
      uint32_t *id_photo;
      printf("Insert keyword you want to search: ");
      fgets(input, MESSAGE_LEN, stdin);
      k_word = malloc(sizeof(input)*sizeof(char));
      sscanf(input, "%s", k_word);
      if((k=gallery_search_photo(peer_socket, k_word, &id_photo)) != 0 ){
        printf("Photos with '%s' as keyword:\n",k_word);
        for(int i=0; i<k; i++){
          printf("\t");
          printf("%"PRIu32"\n", id_photo[i]);
        }
        printf("Number of photos: %d\n\n", k);
      }else if(k ==0){
              printf("No picture exists with that keyword\n");
              printf("Problem making the connection\n");
            }
      break;
    }
    case 'g':
    {
      //Getting necessary information for th function
      printf("Insert the ID of the file you want to download: ");
      fgets(input, MESSAGE_LEN, stdin);
      sscanf(input,"%d",&key_id);
      printf("Insert the name you want to give the photo: ");
      fgets(input, MESSAGE_LEN, stdin);
      k_word = malloc(sizeof(input)*sizeof(char));
      sscanf(input, "%s", k_word);
      //Getting photo
      int pic = gallery_get_photo(peer_socket, key_id, k_word);
      if(pic == 0){
        printf("There is no photo with the inserted ID\n\n");
        break;
      }
      if(pic == -1){
        printf("There was an error while trying to download the picture\n\n");
        break;
      }else{
        printf("Download successful :D\nIt's a boy!\n\n");
        return 1;
        break;
      }
    }
    //Default input hadeling
    default:
      printf("The command you input is not valid\nTry again faggot\n");
    }

    printf("\nChoose one of the following commands\n");
    printf("\ta - Add photo\n");
    printf("\td - Delete photo\n");
    printf("\tk - Add a keyword to the picture \n");
    printf("\ts - Search for photos by keyword\n");
    printf("\tg - Download a photo\n");
    printf("\tq - Exit program \n");
    printf("Command: ");

  }


  //Sending message to peer telling the client is disconnecting
  p.message_type = -99;
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

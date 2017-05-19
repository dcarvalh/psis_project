#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "img_list.h"

#include <inttypes.h>


struct keyword{    //List that will store keyword information
  char * keyword_name;
  struct keyword * next_key;
};

struct photo{    //List that will store photo information
  uint32_t id_photo;
  char * file_name;
  struct keyword * key_head;
  struct photo * next;
};

photolist * InitPhotoList(void)
{
  photolist * new;
  new = NULL;
  return new;
}

photolist * NewPhoto(photolist * head, uint32_t new_id_photo, char * new_file_name)
{
	photolist * new;
  new = (photolist *) malloc(sizeof(photolist));

  strcpy(new->file_name,new_file_name);
  new->id_photo = new_id_photo;
  new->key_head = NULL;

	if(head == NULL){
    new->next = NULL;
  }else{
    new->next = head; // insere no inicio da lista
  }
  head=new;

  return head;
}

void PrintPhotoList(photolist * head)
{

  if(head==NULL){
    printf("Empty photo list\n");
    return;
  }

  photolist * aux;
  aux = head;

  int i=1;
  while(aux != NULL){
    aux=head->next;
    //print struct
    printf("--- photo %d ---\n", i);
    printf("%s\n", head->file_name);
    printf("%" PRIu32 "\n", head->id_photo);
    head=aux;
  }
	return;
}

void FreePhotoList(photolist * head){

  if(head==NULL){
    printf("Empty photo list -  Photo list is freed.\n");
    return;
  }

  photolist * aux;
  aux = head;

  while(aux != NULL){
    aux=head->next;
    free(head);
    head=aux;
  }
  printf("Dobby is free!  -  Photo list is freed.\n");
	return;
}

/*
char *GiveIP(peerlist * head){
	return head->ip;
}

int GivePort(peerlist * head){
	return head->port;
}

peerlist* NextPeer(peerlist *head){
	head = head->next;
	return head;
}
*/
/*
int CountPeers(peerlist *head){
  peerlist *aux;
  aux = head;
  int count=0;

  if(aux == NULL){
    return count;
  }else{
    do{
    ++count;
    aux=aux->next;
    }while(aux != head);
  }
  return count;
}
*/
/*
peerlist * RemovePeer(peerlist *head, char r_ip[MESSAGE_LEN], int r_port){
    peerlist *aux, *aux2;
    aux = head;
    aux2 = aux->next;

    if(aux == NULL){ //empty list
      printf("List is empty! Trying to remove Peer from empty list.\n"); //debug
      return head;
    }

    if(aux == aux2){ //If head is the only server in the list
      printf("Only head in list\n"); //debug
      if(aux->port == r_port && strcmp(aux->ip,r_ip) == 0){
        free(aux);
        head=NULL;
        return head;
      }else{
        printf("Peer you are trying to remove is not in list!\n");
        return head;
      }
    }

    while(aux2 != head){ //if peer to remove is not head
      if(aux2->port == r_port && strcmp(aux2->ip,r_ip)==0){
        aux->next=aux2->next;
        free(aux2);
        return head;
      }else{
        aux=aux->next;
        aux2=aux->next;
      }
    }
    if(aux2->port == r_port && strcmp(aux2->ip,r_ip)==0){    //if peer to remove is head
      aux->next=aux2->next;
      free(aux2);
      head=aux->next;
      return head;
    }else{
      printf("Peer you are trying to remove is not in list !\n");
    }
    return head;
}
*/

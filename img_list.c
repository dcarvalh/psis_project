#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "img_list.h"
#include "message.h"
#include <unistd.h>


#include <inttypes.h>

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

  strcpy(new->file_name, new_file_name);
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
    if(head->key_head != NULL)
      PrintKeyWords(head);
    head=aux;
    i++;
  }
  printf("---------------\n\n");
	return;
}

void FreePhotoList(photolist * head){

  if(head==NULL){
    printf("Empty photo list -  Photo list is freed.\n");
    return;
  }

  photolist * aux;
  aux = head;

  char str[sizeof(uint32_t)]; //var para conversão para char *

  while(aux != NULL){
    aux=head->next;

    sprintf(str, "%d", head->id_photo);
    unlink(str);
    FreeKeywords(head->key_head);
    free(head);
    head=aux;
  }
  printf("Dobby is free!  -  Photo list is freed.\n");
	return;
}

photolist *GetPhoto(photolist *head, uint32_t id){
  photolist *aux;
  for(aux = head; aux!=NULL; aux = aux->next){
    if(aux->id_photo == id){
      return aux;
    }
  }
  return NULL;
}

keyword *NewKeyWord(keyword *key_head, char *new_key_name){
  keyword *new;
  new = (keyword *) malloc (sizeof(keyword));

  strcpy(new->keyword_name, new_key_name);

  if(key_head == NULL){
    new->next_key = NULL;
  }else{
    new->next_key = key_head;
  }
  key_head = new;
  return key_head;
}

keyword *GetKeyHead(photolist *head){
  printf("Keyword added to " "%" PRIu32 "\n", head->id_photo);
  return head->key_head;
}

void PrintKeyWords(photolist *k_head){
  keyword *head;
  printf("keywords: ");
  for(head = k_head->key_head; head != NULL; head = head->next_key){
    printf("%s", head->keyword_name);
    if (head->next_key != NULL)
      printf(", ");
  }
  printf("\n");
}

void Adding(photolist *aux, keyword *k_head){
  aux->key_head = k_head;
  return;
}

int SearchPhotosbyKeyWords(photolist *head, char *kword, uint32_t *photos){
  photolist *aux;
  int n = 0;
  for(aux=head; aux!=NULL; aux=aux->next){
    if(n==50)
      break;
    for(keyword *k_head = aux->key_head; k_head !=NULL; k_head=k_head->next_key){
      if(n==50)
        break;
      if(strcmp(k_head->keyword_name,kword) == 0){
            photos[n]=aux->id_photo;
            n++;
      }
    }
  }

  return n;
}

void FreeKeywords(keyword * head){
  if(head==NULL){
    printf("No keywords\n");
    return;
  }

  keyword * aux;
  aux = head;

  while(aux != NULL){
    aux=head->next_key;
    free(head);
    head=aux;
  }
  printf("Keywords free\n");
	return;

}

photolist * DeletePhoto(photolist *head, photolist *rem){

  char str[sizeof(uint32_t)]; //var para conversão para char *

  if(head==rem){    //remover o head
    head=head->next;
    free(rem->file_name);
    sprintf(str, "%d", rem->id_photo);
    unlink(str);
    FreeKeywords(rem->key_head);
    free(rem);
    return head;
  }

  photolist *aux;
  photolist *aux2;
  aux2=head;
  aux=head->next;

  while(aux != NULL){   //remover outro
    if(aux==rem){
      aux2->next=aux->next;
      free(rem->file_name);
      sprintf(str, "%d", rem->id_photo);
      unlink(str);
      FreeKeywords(rem->key_head);
      free(rem);
      return head;
    }
    aux=aux->next;
    aux2=aux2->next;
  }

  return (photolist *) -1;
}

char *GetPhotoName(photolist *head){
  return head->file_name;
}

photolist *InsertPhotoEnd(photolist *head, uint32_t new_id_photo, char * new_file_name){

  photolist *aux;
  photolist * new;
  new = (photolist *) malloc(sizeof(photolist));

  strcpy(new->file_name, new_file_name);
  new->id_photo = new_id_photo;
  new->key_head = NULL;

  if(head == NULL){
    new->next = NULL;
    head = new;
    return head;
  }else{
    for(aux=head; aux!= NULL; aux=aux->next){
      if(aux->next==NULL){
        aux->next = new;
        break;
      }
    }
  }
  return head;
}

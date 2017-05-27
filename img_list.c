#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "img_list.h"

#include <inttypes.h>


struct keyword{    //List that will store keyword information
  char *keyword_name;
  struct keyword * next_key;
};

struct photo{    //List that will store photo information
  uint32_t id_photo;
  char *file_name;
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

  new->file_name = malloc(strlen(new_file_name)*sizeof(char));
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
    head=aux;
    i++;
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

photolist *GetPhoto(photolist *head, uint32_t id){
  photolist *aux;
  for(aux = head; aux!=NULL; aux= aux->next){
    if(aux->id_photo == id){
      return aux;
    }
  }
  return NULL;
}

keyword *NewKeyWord(keyword *key_head, char *new_key_name){
  keyword *new;
  new = (keyword *) malloc (sizeof(keyword));

  new->keyword_name = malloc(strlen(new_key_name)*sizeof(char));
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
    return head->key_head;
}

void PrintKeyWords(photolist *k_head){
  keyword *head;
  printf("Keywords:\n");
  for(head = k_head->key_head; head != NULL; head = head->next_key)
    printf("  %s\n", head->keyword_name);

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

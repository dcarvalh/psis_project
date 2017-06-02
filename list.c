#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

peerlist * InitList()
{
  peerlist * new;
  new = NULL;
  return new;
}

peerlist * NewPeer(peerlist * head, char newip[MESSAGE_LEN], int newport)
{
	peerlist * new;
  new = (peerlist *) malloc(sizeof(peerlist));

	if(head == NULL){
    strcpy(new->ip,newip);
    new->port = newport;
    new->next = new;
    head=new;
    return head;
  }

  peerlist * aux;
  aux=head->next;

	strcpy(new->ip,newip);
	new->port=newport;

  head->next = new;
	new->next = aux;

	return new;
}

void PrintList(peerlist * head)
{
	peerlist *aux;

	aux = head;

	if(aux == NULL){
		printf("\nNo existing Peers\n");
	}else{
    printf("\nExisting Peers:\n");
    do{
    printf("IP: %s \n Port: %d\n",aux->ip, aux->port);
    aux=aux->next;
    }while(aux != head);
	}

	return;
}

void FreeList(peerlist * head)
{
	peerlist *aux, *aux2;

  if(head!=NULL){
    aux=head->next;
    while(aux != head){
      aux2=aux->next;
      free(aux);
      aux=aux2;
    }
    free(head);
  }

	return;
}

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

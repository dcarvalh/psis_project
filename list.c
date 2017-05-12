#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct list{    //List that will store server information
  char ip[MESSAGE_LEN];
  int port;
  struct list *next;
};

peerlist * InitList(void)
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

/*char *GiveIP(ListaNomes *NameList, char *clientip){
	int n;

	n= sizeof(NameList->name);
	clientip = (char *) malloc(n+1);
	strcpy(clientip, NameList->ip);
	return clientip;
}
*/

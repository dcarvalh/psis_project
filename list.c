#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct list{    //List that will store server information
  char ip[MESSAGE_LEN];
  int port;
  struct list *next;
};

///////////////////////////////
struct message{             //
  char addr[MESSAGE_LEN];  //
  int message_type;       //
  int port;              //
};                      //
/////////////////////////
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
		printf("Lista Vazia!\n");
	}else{
    printf("---------------Lista-------------\n");
    do{
    printf("IP: %s \n Port: %d\n",aux->ip, aux->port);
    aux=aux->next;
    }while(aux != head);
	}
	printf("-------------Fim da Lista-------------\n");

	return;
}

///////////////////////////////////////////////
message FillMessage(peerlist *head){        //
  message m;                               //
  strcpy(m.addr, head->ip);               //
  m.port = head->port;                   //
                                        //
  return m;                            //
}                                     //
///////////////////////////////////////

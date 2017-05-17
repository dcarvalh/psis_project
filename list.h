#define MESSAGE_LEN 20

typedef struct list peerlist;

/*typedef struct message message;*/

peerlist * InitList(void);

peerlist * NewPeer(peerlist * head, char newip[MESSAGE_LEN], int newport);

void PrintList(peerlist * head);

void FreeList(peerlist * head);

char *GiveIP(peerlist * head);

int GivePort(peerlist * head);

peerlist* NextPeer(peerlist *head);

int CountPeers(peerlist *head);

void RemovePeer(peerlist *head, char r_port[MESSAGE_LEN], int r_ip);

////////////////////////////////////////
/*message FillMessage(peerlist *head);  /*/
///////////////////////////////////////
/*
ListaNomes * FindName(ListaNomes * NameList, char Name[30]);

int QuitName(ListaNomes *CleanNameList, char Name[30]);

char *ReturnIP(ListaNomes *NameList, char *clientip);
int ReturnPort(ListaNomes *NameList);

ListaNomes* NextElement(ListaNomes *NameList);

void FreeList(ListaNomes *NameList);*/

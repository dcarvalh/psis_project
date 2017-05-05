#define MESSAGE_LEN 20

typedef struct list peerlist;



peerlist * InitList(void);

peerlist * NewPeer(peerlist * head, char newip[MESSAGE_LEN], int newport);

void PrintList(peerlist * head);

/*
ListaNomes * FindName(ListaNomes * NameList, char Name[30]);

int QuitName(ListaNomes *CleanNameList, char Name[30]);

char *ReturnIP(ListaNomes *NameList, char *clientip);
int ReturnPort(ListaNomes *NameList);

ListaNomes* NextElement(ListaNomes *NameList);

void FreeList(ListaNomes *NameList);*/

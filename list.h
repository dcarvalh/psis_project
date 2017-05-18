#define MESSAGE_LEN 20

typedef struct list peerlist;

peerlist * InitList(void);

peerlist * NewPeer(peerlist * head, char newip[MESSAGE_LEN], int newport);

void PrintList(peerlist * head);

void FreeList(peerlist * head);

char *GiveIP(peerlist * head);

int GivePort(peerlist * head);

peerlist* NextPeer(peerlist *head);

int CountPeers(peerlist *head);

peerlist * RemovePeer(peerlist *head, char r_port[MESSAGE_LEN], int r_ip);

#define MESSAGE_LEN 20
#include <stdint.h>
#include <ctype.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/stat.h>
#define BICHO printf("Bichooooooooooooooooooooooooooooooo!\n");
typedef struct message{
  char addr[MESSAGE_LEN];
  int message_type;
  int port;
}message;

/*Message Type:
*  19- New peer connected
*  5 - Client removes picture from server
*  3 - Client adding keyword
*  2 - Client Seding picture
*  1 - Server connect
*  0 - Exists a peer for client / Client asks the gateway for peer information
* -1 - Server Disconnecting
* -99 - Client Disconnected from server
* -19 - Peer left
*/

typedef struct picture_info{
  int message_type;
  uint32_t size;
  char pic_name[MESSAGE_LEN];
}pic_info;


int send_all(int sock, const void *buff, size_t length, int flags);

int recv_all(int sock, void *buff, size_t length, int flags);

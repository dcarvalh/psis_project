#define MESSAGE_LEN 20
#define BICHO   printf("KAKAKAKAKAKAKAKAKAK4AKAKAKAKAKAKA!");
#include <stdint.h>
typedef struct message{
  char addr[MESSAGE_LEN];
  int message_type;
  int port;
}message;

/*Message Type:
*  5 - Client removes picture from server
*  3 - Client adding keyword
*  2 - Client Seding picture
*  1 - Server connect
*  0 - Exists a peer for client / Client asks the gateway for peer information
* -1 - Server Disconnecting
* -99 - Client Disconnected from server
*/

typedef struct picture_info{
  int message_type;
  uint32_t size;
  char pic_name[MESSAGE_LEN];
}pic_info;

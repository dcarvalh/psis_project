#define MESSAGE_LEN 20

typedef struct message{
  char addr[MESSAGE_LEN];
  int message_type;
  int port;
}message;

/*Message Type:
* 2 - Client Seding picture
* 1 - Server connect
* 0 - Existe server para o client / CLiente pede peer ao gateway
* -1 - Server Disconnecting
*/

typedef struct picture_info{
  int message_type;
  int size;
  char pic_name[MESSAGE_LEN];
}pic_info;

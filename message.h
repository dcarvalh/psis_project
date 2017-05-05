#define MESSAGE_LEN 20

typedef struct message{
  char addr[MESSAGE_LEN];
  int message_type;
  int port;
}message;

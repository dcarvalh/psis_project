#include "storyserver.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <arpa/inet.h>
#define MAX_WAIT_LIST 5


//Variávies globais
int sock_fd;
char * story;
struct sigaction * handler;

//Função exit_server - fecha socket e faz exit
void exit_server() {
	printf("\n%s\n", story);
	close(sock_fd);
	//unlink(SOCKET_NAME);
	free(handler);
	exit(0);
}

int main(){
  struct sockaddr_in server_addr={.sin_family=AF_INET, .sin_port= htons(3000), .sin_addr.s_addr= INADDR_ANY};
  struct sockaddr_in client_addr;
  socklen_t addrlen;

	int yes=1;

  //srand(time(NULL));
  handler = malloc(sizeof(handler));
  handler->sa_handler = &exit_server;

  message m;
  story = strdup("");
  char message[50];
  sprintf(message, "Buffer is full");

  //criar socket e bind
  sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("error creating socket \n ");
		exit(-1);
	}
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("Setsockopt error");
		exit(-1);
	}

	int err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(err == -1) {
		perror("error in bind \n");
		exit(-1);
	}
  printf(" socket created and binded\n Ready to receive messages\n");

	sigaction(SIGINT, handler, NULL);

	listen(sock_fd, MAX_WAIT_LIST);

  while(1){

		int new_sock= accept(sock_fd, (struct sockaddr *) & client_addr, &addrlen);
    printf("aceite!\n");
    //Read message
    recv(new_sock, m.buffer, sizeof(m.buffer), 0);
    //Process message
    story = strcat(story, m.buffer);
    //Sends story
    send(new_sock, story, sizeof(story), 0);
    close(new_sock);

		printf("%s\n", story);

  }

  printf("OK\n");

  close(sock_fd);
  exit(0);

}

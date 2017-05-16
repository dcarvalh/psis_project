#include "storyserver.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

int main(){

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
	// these values can be read from the keyboard
	server_addr.sin_port= htons(3000);
	inet_aton("127.0.0.1", &server_addr.sin_addr);


  message m;
  char * story;

	char client_name[50];
  sprintf(client_name, "./sock_cli_%d", getpid());
  /*strcpy(client_addr.sin_path, client_name);*/

  int sock_fd= socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1){
  	perror("error creating socket \n");
  	exit(-1);
  }

  if( -1 == connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ){
    printf("Error connecting\n");
    exit(-1);
	}


  printf(" socket created and connected\n Ready to send\n");

  //write message
  printf("message: ");
  fgets(m.buffer, MESSAGE_LEN, stdin);
  send(sock_fd, m.buffer, sizeof(m.buffer), 0);
  //receive story
  recv(sock_fd, m.buffer, sizeof(m.buffer), 0);

  printf("%s\n", m.buffer);

  printf("OK\n");
  close(sock_fd);
  exit(0);

}

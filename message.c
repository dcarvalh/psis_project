#include "message.h"
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

int send_all(int sock, const void *buff, size_t length, int flags){
	ssize_t nbytes;
	const char *p = buff;
	while (length > 0){
		nbytes = send(sock, p, length, flags);
		if (nbytes <= 0) break;
		p += nbytes;
		length -= nbytes;
	}
  if(nbytes <= 0)
    return -1;
  else
    return 0;
}

int recv_all(int sock, void *buff, size_t length, int flags){
	ssize_t nbytes;
	int total_bytes = 0;
	char *p = buff;
	while (total_bytes < length){
		nbytes = recv(sock, p, length, flags);
		if (nbytes <= 0) break;
		p += nbytes;
		total_bytes += nbytes;
	}
	if(total_bytes > length)
		return -2; //overflow of buff
	if(nbytes <= 0)
		return nbytes; //error or connection closed
	else
		return total_bytes;
}

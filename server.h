#ifndef SERVER

#define SERVER

#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Server {
	int socketFileDescriptor;
	struct sockaddr_in socketAddress;
} Server;

void initServer(Server* server);

#endif
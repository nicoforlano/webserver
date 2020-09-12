#ifndef SERVER

#define SERVER

#define _POSIX_C_SOURCE 200112L

#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Server {
	int socketFileDescriptor;
	struct sockaddr_in* socketAddress;
} Server;

void serverInit(Server* server, char* port);
void serverListen(Server* server);
struct addrinfo* getServerAddressInfo(char* port);

#endif
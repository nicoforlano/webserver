#ifndef SERVER

#define SERVER
#define _POSIX_C_SOURCE 200112L
#define BACKLOG 20
#define TRUE 1

#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

typedef struct Server {
	int socketFileDescriptor;
	struct sockaddr_in* socketAddress;
} Server;

typedef struct Request {
	int fileDescriptor;
	struct sockaddr* addressInfo;
} Request;

void serverInit(Server* server, char* port);
void serverListen(Server* server);
struct addrinfo* getServerAddressInfo(char* port);
void createRequestThread(Request* request);
void initRequestThreadAttributes(pthread_attr_t* attributes);
void* handleRequest(void* args);

#endif
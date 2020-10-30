#ifndef SERVER

#define SERVER
#define _POSIX_C_SOURCE 200112L
#define BACKLOG 20
#define TRUE 1
#define PORT_ARG_INDEX 1

#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <argp.h>
#include <fcntl.h>

typedef struct Server {
	int socketFileDescriptor;
	struct sockaddr_in* socketAddress;
} Server;

typedef struct Request {
	int fileDescriptor;
	struct sockaddr* addressInfo;
} Request;

typedef struct Config {
	char listeningPort[5];
	int mode;
} Config;

typedef enum {
	NO_MODE,
	BLOCKING,
	NONBLOCKING
} ServerMode;

void serverInit(Server* server, Config* config);
void serverListen(Server* server, int serverMode);
void nonBlockingServerListen(Server* server);
struct addrinfo* getServerAddressInfo(char* port);
void createRequestThread(Request* request);
void initRequestThreadAttributes(pthread_attr_t* attributes);
void* handleRequest(void* args);
void configInit(Config* config, int argc, char *argv[]);

#endif
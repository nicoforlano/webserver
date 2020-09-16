#include "server.h"


void serverInit(Server* server, char* port) {

	struct addrinfo* serverAddressInfo;

	serverAddressInfo = getServerAddressInfo(port);

	printf("%d - %d - %d \n", serverAddressInfo->ai_family, serverAddressInfo->ai_socktype, serverAddressInfo->ai_protocol);
	server->socketFileDescriptor = socket(serverAddressInfo->ai_family, 
										  serverAddressInfo->ai_socktype, 
										  serverAddressInfo->ai_protocol);
	server->socketAddress = (struct sockaddr_in*) serverAddressInfo->ai_addr;

	printf("** Socket Server File Descriptor: %d\n **", server->socketFileDescriptor);

	if(server->socketFileDescriptor == -1) {
		printf("** Error - Socket couldn't be created: %s %d**\n", gai_strerror(errno), errno);
		exit(0);
	}

	bind(server->socketFileDescriptor, serverAddressInfo->ai_addr, serverAddressInfo->ai_addrlen);

}

struct addrinfo* getServerAddressInfo(char* port) {

	int status;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	struct addrinfo* serverAddressInfo = malloc(sizeof(struct addrinfo*));

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((status = getaddrinfo(NULL, port, &hints, &serverInfo)) != 0) {
		fprintf(stderr, "* Error while trying to get server information: %s\n", gai_strerror(status));
		exit(1);
	}

	for(struct addrinfo *currentAddress = serverInfo; currentAddress != NULL; currentAddress = currentAddress->ai_next) {
		if(currentAddress->ai_family == AF_INET) {
			serverAddressInfo = currentAddress;
			printf("Setting address - %d - %d - %d\n", serverAddressInfo->ai_family, serverAddressInfo->ai_socktype, serverAddressInfo->ai_protocol);
		}
	}

	if(serverAddressInfo == NULL) {
		printf("Server address information couldn't be obtained\n");
		exit(1);
	}

	//freeaddrinfo(serverInfo);

	return serverAddressInfo;
}

void serverListen(Server* server) {

	Request* request = (Request*) malloc(sizeof(Request*));
	socklen_t requestAddressSize = sizeof(request->addressInfo);

	listen(server->socketFileDescriptor, BACKLOG);

	while(TRUE) {

		request->fileDescriptor = accept(server->socketFileDescriptor, 
												  request->addressInfo, 
												  &requestAddressSize);
		printf("accepting\n");
		if(request->fileDescriptor < 0) {
			printf("* Error while accepting new connection: %s\n", strerror(errno));
		}

		createRequestThread(request);

	}
}

void createRequestThread(Request *request) {

	pthread_t requestThread;
	pthread_attr_t requestThreadAttributes;

	initRequestThreadAttributes(&requestThreadAttributes);
	printf("Creating Thread\n");
	pthread_create(&requestThread, &requestThreadAttributes, handleRequest, request);

}

void initRequestThreadAttributes(pthread_attr_t* attributes) {

	pthread_attr_init(attributes);
	pthread_attr_setdetachstate(attributes, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(attributes, SCHED_FIFO);

}

void* handleRequest(void* args) {

	printf("Hndling request!\n");
}

void configInit(Config* config, int argumentsCount, char *arguments[]) {

	config->listeningPort = arguments[PORT_ARG_INDEX];
	if(atoi(config->listeningPort) == 0) {
		printf("Error: Port number is not valid.\n");
		exit(1);
	}
}
#include "server.h"
#include "blockingServer.h"
#include "nonBlockingServer.h"

void serverInit(Server* server, Config* config) {

	printf("*** Initializing server on port %s***\n", config->listeningPort);

	struct addrinfo* serverAddressInfo;

	serverAddressInfo = getServerAddressInfo(config->listeningPort);

	server->socketFileDescriptor = socket(serverAddressInfo->ai_family, 
										  serverAddressInfo->ai_socktype, 
										  serverAddressInfo->ai_protocol);
	server->socketAddress = (struct sockaddr_in*) serverAddressInfo->ai_addr;

	if(server->socketFileDescriptor == -1) {
		printf("** Error - Socket couldn't be created: %s %d**\n", gai_strerror(errno), errno);
		exit(0);
	}

	if(config->mode == NONBLOCKING) {
		printf("Setting non block\n");
		fcntl(server->socketFileDescriptor, F_SETFL, O_NONBLOCK);
	}

	bind(server->socketFileDescriptor, serverAddressInfo->ai_addr, serverAddressInfo->ai_addrlen);
	
	freeaddrinfo(serverAddressInfo);

}

struct addrinfo* getServerAddressInfo(char* port) {

	int status;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	struct addrinfo* serverAddressInfo;

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
		}
	}

	if(serverAddressInfo == NULL) {
		printf("Server address information couldn't be obtained\n");
		exit(1);
	}

	return serverAddressInfo;
}

void serverListen(Server* server, Config* config) {

	listen(server->socketFileDescriptor, BACKLOG);

	switch(config->mode) {
		case BLOCKING: {
			printf("*** Server listening ***\n");
			startBlockingRequestHandling(server, config);
		}
		case NONBLOCKING: {
			printf("Non blocking server listening\n");
			startNonBlockingRequestHandling(server);
		}
	}
	
}

void configInit(Config* config, int argumentsCount, char *arguments[]) {

	if(atoi(arguments[PORT_ARG_INDEX + 1]) == 1) {
		if(argumentsCount < 4) {
			printf("> Please specify the listenting port, server's mode and worker thread count. Ex: 8080 1 20\n");
			exit(0);
		}

		config->workerThreadsCount = atoi(arguments[PORT_ARG_INDEX + 2]);

	} else {
		if(argumentsCount < 3) {
			printf("> Please specify the listenting port and server's mode. Ex: 8080 1\n");
			exit(0);
		}
	}
	
	strcpy(config->listeningPort, arguments[PORT_ARG_INDEX]);

	if(atoi(config->listeningPort) == 0) {
		printf("Error: Port number is not valid.\n");
		exit(1);
	}

	config->mode = atoi(arguments[PORT_ARG_INDEX + 1]);

}
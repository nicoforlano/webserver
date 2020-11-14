#include "server.h"
#include "blockingServer.h"
#include "nonBlockingServer.h"


Config* initializeConfigDefaults() {
	Config* config = malloc(sizeof(Config));
	strcpy(config->listeningPort, "8080");
	config->mode = 1;
	config->workerThreadsCount = 10;
	return config;
}

void showServerConfig(Config* config) {
	printf("***** SERVER CONFIGURATION *****\n");
	printf("*** PORT: %s \n", config->listeningPort);
	printf("*** MODE: %s\n", (config->mode == BLOCKING) ? "BLOCKING" : "NONBLOCKING");
	printf("*** WORKER THREADS: %d\n", (config->mode == BLOCKING) ? config->workerThreadsCount : 0);
	printf("*********************************\n");
}

void serverInit(Server* server, Config* config) {

	struct addrinfo* serverAddressInfo;

	serverAddressInfo = getServerAddressInfo(config->listeningPort);

	server->socketFileDescriptor = socket(serverAddressInfo->ai_family, 
										  serverAddressInfo->ai_socktype, 
										  serverAddressInfo->ai_protocol);
	server->socketAddress = (struct sockaddr_in*) serverAddressInfo->ai_addr;

	if(server->socketFileDescriptor == -1) {
		printf("> Error - Socket couldn't be created: %s %d**\n", gai_strerror(errno), errno);
		exit(0);
	}

	if(config->mode == NONBLOCKING) {
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
		fprintf(stderr, "> Error while trying to get server information: %s\n", gai_strerror(status));
		exit(1);
	}

	for(struct addrinfo *currentAddress = serverInfo; currentAddress != NULL; currentAddress = currentAddress->ai_next) {
		if(currentAddress->ai_family == AF_INET) {
			serverAddressInfo = currentAddress;
		}
	}

	if(serverAddressInfo == NULL) {
		printf("> Server address information couldn't be obtained\n");
		exit(1);
	}

	return serverAddressInfo;
}

void serverListen(Server* server, Config* config) {
	listen(server->socketFileDescriptor, BACKLOG);
	printf("> Server is listening incoming requests...\n");
	switch(config->mode) {
		case BLOCKING: {
			startBlockingRequestHandling(server, config);
		}
		case NONBLOCKING: {
			startNonBlockingRequestHandling(server);
		}
	}
}


void parseHttpRequestLine(Request *request, char *buffer) {

}

void parseHttpMethod(Request *request, char *line) {
	
}

char createHttpResponse() {


}
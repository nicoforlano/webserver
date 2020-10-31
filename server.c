#include "server.h"


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

void serverListen(Server* server, int serverMode) {

	listen(server->socketFileDescriptor, BACKLOG);

	switch(serverMode) {
		case BLOCKING: {
			printf("*** Server listening ***\n");

			while(TRUE) {

				Request* request = malloc(sizeof(Request));
				request->addressInfo = malloc(sizeof(struct sockaddr));
				socklen_t requestAddressSize = sizeof(request->addressInfo);

				request->fileDescriptor = accept(server->socketFileDescriptor, 
														  request->addressInfo, 
														  &requestAddressSize);
				if(request->fileDescriptor < 0) {
					printf("> Error while accepting new connection: %s\n", strerror(errno));
				}

				createRequestThread(request);

			}	
		}
		case NONBLOCKING: {

			printf("Non blocking\n");
			char buffer[256];
			struct timeval tv;
			fd_set readFds;
			fd_set tempReadFds;
			int maxFd = server->socketFileDescriptor;

			tv.tv_sec = 2;
			tv.tv_usec = 500000;

			FD_ZERO(&readFds);
			FD_SET(server->socketFileDescriptor, &readFds);

			while(TRUE) {

				tempReadFds = readFds;
				printf("maxFd %d\n", maxFd);
				int selectReturn = select(maxFd + 1, &tempReadFds, NULL, NULL, NULL);
				printf("Select return\n");

				if(selectReturn == -1) {
					perror("select");
				}

				for (int fd = 0; fd <= maxFd; ++fd)
				{
					if(FD_ISSET(fd, &tempReadFds)) {
						printf("Evento en descriptor %d\n", fd);

						if(fd == server->socketFileDescriptor) {

							struct sockaddr clientAddress;
							socklen_t clientAddressLength;

							int clientFd = accept(server->socketFileDescriptor, &clientAddress, &clientAddressLength);

							printf("Accepteed new connection %d\n", clientFd);

							FD_SET(clientFd, &readFds);

							if(clientFd > maxFd) {
								maxFd = clientFd;
							}
						} else {

							memset(&buffer, 0, 256);
							ssize_t readed = recv(fd, &buffer, 256, 0);
							if(readed < 1) {
								FD_CLR(fd, &readFds);
								continue;
							} else {
								printf("Client %d: %s\n", fd, buffer);
							}
						}
					}
				}
			}

		}
	}
	
}

void createRequestThread(Request *request) {

	pthread_t requestThread;
	pthread_attr_t requestThreadAttributes;
	initRequestThreadAttributes(&requestThreadAttributes);
	pthread_create(&requestThread, &requestThreadAttributes, handleRequest, request);

}

void initRequestThreadAttributes(pthread_attr_t* attributes) {

	pthread_attr_init(attributes);
	pthread_attr_setdetachstate(attributes, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(attributes, SCHED_FIFO);

}

void* handleRequest(void* args) {
	
	Request* request = (Request*) args;

	printf("Hndling request - FD: %d!\n", request->fileDescriptor);
	
	char *msg = "HELLO MY FELLOW CLIENT!";

	send(request->fileDescriptor, msg, strlen(msg), 0);

	printf("Closing connection %d\n", request->fileDescriptor);

	close(request->fileDescriptor);

	free(request->addressInfo);
	free(request);

	return 0;
}

void nonBlockingServerListen(Server* server) {

	
}

void configInit(Config* config, int argumentsCount, char *arguments[]) {

	if(argumentsCount < 3) {
		printf("> Please specify the listenting port and the mode. Ex: 8080 1\n");
		exit(0);
	}
	
	strcpy(config->listeningPort, arguments[PORT_ARG_INDEX]);

	if(atoi(config->listeningPort) == 0) {
		printf("Error: Port number is not valid.\n");
		exit(1);
	}

	config->mode = atoi(arguments[PORT_ARG_INDEX + 1]);

}
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

unsigned long getFileSize(char* fileName) {
	FILE* file = fopen(fileName, "r");
	if(file == NULL) {
		return 0;
	}
	fseek(file, 0, SEEK_END);
	unsigned long size = (unsigned long) ftell(file);
	fseek(file, 0, SEEK_SET);
	fclose(file);
	return size;
}

void handleHttpRequest(int requestFd) {

	char buffer[MAX_BUFFER_LENGTH];

	recv(requestFd, buffer, MAX_BUFFER_LENGTH, 0);

	printf("HTTP Request:\n%s\n", buffer);

	if(strstr(buffer, "GET / HTTP/1.1") != NULL) {

		sendHttpResponse(requestFd, "200 OK", "text/html; charset=UTF-8", "content/index.html");

	} else if(strstr(buffer, "GET /image.jpg HTTP/1.1") != NULL) {

		sendHttpResponse(requestFd, "200 OK", "image/jpeg", "content/image.jpg");

	} else if(strstr(buffer, "GET /favicon.ico HTTP/1.1") != NULL) {

		sendHttpResponse(requestFd, "200 OK", "image/x-icon", "content/favicon.ico");

	} else if(strstr(buffer, "GET /styles.css HTTP/1.1") != NULL) {

		sendHttpResponse(requestFd, "200 OK", "text/css", "content/styles.css");

	} else {

		sendHttpResponse(requestFd, "404 NOT FOUND", "text/html; charset=UTF-8", "content/not-found.html");

	}

	printf("Closing connection: %d\n\n", requestFd);

	close(requestFd);
}

void sendHttpResponse(int requestFd, char* status, char* contentType, char* fileName) {

		char buffer[MAX_BUFFER_LENGTH];
		FILE *responseFile;
		unsigned long responseFileSize;
		char* httpHeader;
		char* httpResponse;
		int httpResponseSize;

		responseFile = fopen(fileName, "r");
		
		if(responseFile == NULL) {
			httpHeader = "HTTP/1.1 500 INTERNAL SERVER ERROR\r\nConnection: close\r\n\r\n";
			httpResponseSize = strlen(httpHeader);
			send(requestFd, httpHeader, strlen(httpHeader), 0);
			return;
		}
		
		httpHeader = "HTTP/1.1 %s\r\nContent-Length: %d\r\nContent-Type: %s\r\nConnection: close\r\n\r\n";
		responseFileSize = getFileSize(fileName);
		httpResponseSize = strlen(httpHeader) + sizeof(responseFileSize) + strlen(status) + strlen(contentType);
		httpResponse = malloc(httpResponseSize);
		
		sprintf(httpResponse, httpHeader, status, responseFileSize, contentType);
		printf("Sending HTTP Response:\n%s\n", httpResponse);
		send(requestFd, httpResponse, strlen(httpResponse), 0);
		
		fread(buffer, sizeof(char), responseFileSize, responseFile);
		send(requestFd, buffer, responseFileSize, 0);

		fclose(responseFile);
		free(httpResponse);

}
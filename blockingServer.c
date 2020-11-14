#include "blockingServer.h"

void startBlockingRequestHandling(Server* server, Config* config) {
	
	threadpool_t* threadPool = threadpool_create(config->workerThreadsCount,
												 THREADPOOL_QUEUE_SIZE,
												 0);

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

		threadpool_add(threadPool, &handleRequest, request, 0);
	}
}

unsigned long getFileSize(char* fileName) {

	FILE* file = fopen(fileName, "r");
	if(file == NULL) {
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	unsigned long size = (unsigned long) ftell(file);
	fseek(file, 0, SEEK_SET);
	fclose(file);
	return size;
}

void* handleRequest(void* args) {
	// TODO: Serious imlementetion hehe
	Request* request = (Request*) args;
	char buffer[100000];
	FILE *responseFile = malloc(sizeof(FILE));
	unsigned long responseFileSize;
	char* httpHeader;
	char* httpResponse;
	int httpResponseSize;
	printf("Hndling request - FD: %d!\n", request->fileDescriptor);

	recv(request->fileDescriptor, buffer, REQUEST_BUFFER_LENGTH, 0);

	printf("Message recieved from server:\n%s\n", buffer);

	if(strstr(buffer, "GET / HTTP/1.1") != NULL) {

		printf("REQUEST INDEX\n");

		responseFile = fopen("content/index.html", "r");
		if(responseFile == NULL) {
			printf("NULL INDEX\n");
		}
		
		httpHeader = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
		responseFileSize = getFileSize("content/index.html");
		
		httpResponseSize = strlen(httpHeader) + sizeof(responseFileSize);
		httpResponse = malloc(httpResponseSize);
		
		sprintf(httpResponse, httpHeader, responseFileSize);
		printf("Sending %s\n", httpResponse);
		send(request->fileDescriptor, httpResponse, strlen(httpResponse), 0);
		
		fread(buffer, sizeof(char), 100000, responseFile);
		send(request->fileDescriptor, buffer, strlen(buffer), 0);

		close(request->fileDescriptor);
		fclose(responseFile);

	} else if(strstr(buffer, "GET /iadsamage.jpg HTTP/1.1") != NULL) {
		FILE *image;

		image = fopen("/home/nforla/Documentos/ProgamacionRedes/web-server/content/cat.jpg", "r");
		if(image == NULL) {
			printf("NULL IMAGE\n");
		}

		fseek(image, 0L, SEEK_END);
		int size = ftell(image);
		fseek(image, 0L, SEEK_SET);

		httpHeader = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: image/jpeg\r\nConnection: close\r\n\r\n";
		char fileBuff[REQUEST_BUFFER_LENGTH];
		httpResponseSize = strlen(httpHeader) + sizeof(size);
		httpResponse = malloc(httpResponseSize);
		memset(httpResponse, 0, httpResponseSize);
		sprintf(httpResponse, httpHeader, size);

		//char *msg = "HELLO MY FELLOW CLIENT!";
		printf("Sending %s\n", httpResponse);
		send(request->fileDescriptor, httpResponse, httpResponseSize, 0);

		fread(fileBuff, sizeof(char), size, image);

		send(request->fileDescriptor, fileBuff, size+1, 0);

		close(request->fileDescriptor);
		fclose(responseFile);
	}

	
	printf("Closing connection %d\n", request->fileDescriptor);

	
	free(request->addressInfo);
	free(request);

	return 0;
}
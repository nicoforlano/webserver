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
	
	char buffer[MAX_BUFFER_LENGTH];

	Request* request = (Request*) args;

	recv(request->fileDescriptor, buffer, MAX_BUFFER_LENGTH, 0);

	printf("HTTP Request:\n%s\n", buffer);

	if(strstr(buffer, "GET / HTTP/1.1") != NULL) {

		sendHttpResponse(request->fileDescriptor, "200 OK", "text/html; charset=UTF-8", "content/index.html");

	} else if(strstr(buffer, "GET /image.jpg HTTP/1.1") != NULL) {

		sendHttpResponse(request->fileDescriptor, "200 OK", "image/jpeg", "content/image.jpg");

	} else if(strstr(buffer, "GET /favicon.ico HTTP/1.1") != NULL) {

		sendHttpResponse(request->fileDescriptor, "200 OK", "image/x-icon", "content/favicon.ico");

	} else {

		sendHttpResponse(request->fileDescriptor, "404 NOT FOUND", "text/html; charset=UTF-8", "content/not-found.html");

	}

	printf("Closing connection: %d\n\n", request->fileDescriptor);

	close(request->fileDescriptor);
	free(request->addressInfo);
	free(request);

	return 0;
}
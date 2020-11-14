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

void* handleRequest(void* args) {
	
	char buffer[MAX_BUFFER_LENGTH];

	Request* request = (Request*) args;

	handleHttpRequest(request->fileDescriptor);

	free(request->addressInfo);
	free(request);

	return 0;
}
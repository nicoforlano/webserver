#include "blockingServer.h"

void startBlockingRequestHandling(Server* server, Config* config) {

	printf("Worker threads count %d\n", config->workerThreadsCount);
	
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
	// TODO: Serious imlementetion hehe
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
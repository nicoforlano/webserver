#include "blockingServer.h"

void startBlockingRequestHandling(Server* server) {
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
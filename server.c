#include "server.h"


void initServer(Server* server, char* port) {

	int status;
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	struct addrinfo* serverAddressInfo = NULL;

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

	server = (Server*) malloc(sizeof(Server*));

	server->socketFileDescriptor = socket(serverAddressInfo->ai_family, serverAddressInfo->ai_socktype, serverAddressInfo->ai_protocol);
	
	printf("** Socket Server File Descriptor: %d\n **", server->socketFileDescriptor);
	if(server->socketFileDescriptor == -1) {
		printf("** El socket no pudo ser creado **\n");
		exit(0);
	}


}

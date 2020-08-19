#include "server.h"

void initServer(Server* server) {

	server = (Server*) malloc(sizeof(Server));

	server->socketFileDescriptor = socket(PF_INET, SOCK_STREAM, 0);
	
	printf("** Socket Server File Descriptor: %d\n **", server->socketFileDescriptor);
	if(server->socketFileDescriptor == -1) {
		printf("** El socket no pudo ser creado **\n");
		exit(0);
	}


}

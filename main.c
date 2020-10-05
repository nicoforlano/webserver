#include <stdlib.h>
#include <stdio.h>
#include "server.h"

int main(int argc, char *argv[])
{
	printf("**** Setting up server... ****\n");
	
	Server server;
	Config* config = malloc(sizeof(Config));

	configInit(config, argc, argv);
	printf("PORT: %s\n",config->listeningPort);
	serverInit(&server, config->listeningPort);
	serverListen(&server);
	
	printf("Ending\n");
	return 0;
}
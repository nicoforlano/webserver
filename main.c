#include <stdlib.h>
#include <stdio.h>
#include "server.h"

int main(int argc, char *argv[])
{	
	Server server;
	Config* config = malloc(sizeof(Config));

	configInit(config, argc, argv);
	serverInit(&server, config->listeningPort);
	serverListen(&server);
	
	free(config);

	return 0;
}
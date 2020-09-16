#include <stdlib.h>
#include <stdio.h>
#include "server.h"

int main(int argc, char *argv[])
{
	printf("**** INICIANDO ****\n");
	
	Server server;
	Config config;

	configInit(&config, argc, argv);
	printf("PORT: %d\n",config.listeningPort);
	serverInit(&server, config.listeningPort);
	serverListen(&server);
	
	printf("Ending\n");
	return 0;
}
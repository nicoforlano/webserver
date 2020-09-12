#include <stdlib.h>
#include <stdio.h>
#include "server.h"

int main(int argc, char const *argv[])
{
	printf("**** INICIANDO ****\n");
	
	Server* server;

	serverInit(server, "8080");

	printf("Ending\n");
	return 0;
}
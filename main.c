#include <stdlib.h>
#include <stdio.h>
#include "server.h"


const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "<nicoforlanop@gmail.com>";
static char doc[] = "Un webserver muy simple escrito en C como trabajo práctico de la materia Programación en Redes - Universidad de Palermo";

int main(int argc, char *argv[])
{	
	Server server;
	Config* config = malloc(sizeof(Config));

	argp_parse(0, argc, argv, 0, 0, 0);
	
	configInit(config, argc, argv);
	serverInit(&server, config->listeningPort);
	serverListen(&server);
	
	free(config);

	return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include "server.h"


const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "<nicoforlanop@gmail.com>";
static char doc[] = "Un webserver muy simple escrito en C como trabajo práctico de la materia Programación en Redes - Universidad de Palermo";
static struct argp_option options[] = {
	{"port", 'p', "PORT", 0, "Server's listening port."},
	{"mode", 'm', "MODE", 0, "Server's mode: 1 --> Blocking | 2 --> NonBlocking"},
	{"workers", 'w', "WORKER_THREADS_COUNT", 0, "Used if server's mode is blocking, specifies workers count for thread pool."}
};

static error_t parse_opt(int key, char* arg, struct argp_state *state) {

	Config *config = state->input;
	int serverMode;

	switch(key) {
		case 'p':
			if(atoi(arg) == 0) {
				printf("Error: Port number is not specified or valid.\n");
				exit(1);
			}
			strcpy(config->listeningPort, arg);
			break;
		case 'm':
			serverMode = atoi(arg);
			if(serverMode != BLOCKING && serverMode != NONBLOCKING) {
				printf("Error: Server mode is not valid\n");	
				exit(1);
			}
			config->mode = serverMode;
			break;
		case 'w':
			if(config->mode == BLOCKING) {
				int workersCount = atoi(arg);
				if(workersCount < 1) {
					printf("Error: Threadpool Workers threads count is not valid\n");
					exit(1);
				}
				config->workerThreadsCount = workersCount;
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp argp = {options, parse_opt, NULL, doc};


int main(int argc, char *argv[])
{	
	Server server;
	Config* config = initializeConfigDefaults();

	argp_parse(&argp, argc, argv, 0, 0, config);
	showServerConfig(config);
	serverInit(&server, config);
	serverListen(&server, config);
	
	free(config);

	return 0;
}
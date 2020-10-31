#ifndef BLOCKING_SERVER

#define THREADPOOL_QUEUE_SIZE 10

#include "server.h"
#include "threadpool.h"

void startBlockingRequestHandling(Server* server, Config* config);
void* handleRequest(void* args);

#endif
#ifndef BLOCKING_SERVER

#define THREADPOOL_QUEUE_SIZE 10

#include "server.h"
#include "threadpool.h"

void startBlockingRequestHandling(Server* server);
void createRequestThread(Request* request);
void initRequestThreadAttributes(pthread_attr_t* attributes);
void* handleRequest(void* args);

#endif
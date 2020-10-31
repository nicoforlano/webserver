#ifndef BLOCKING_SERVER

#include "server.h"

void startBlockingRequestHandling(Server* server);
void createRequestThread(Request* request);
void initRequestThreadAttributes(pthread_attr_t* attributes);
void* handleRequest(void* args);

#endif
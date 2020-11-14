#include "nonBlockingServer.h"


void startNonBlockingRequestHandling(Server* server) {

	char buffer[256];
	fd_set readFds;
	fd_set tempReadFds;
	int maxFd = server->socketFileDescriptor;

	FD_ZERO(&readFds);
	FD_SET(server->socketFileDescriptor, &readFds);

	while(TRUE) {

		tempReadFds = readFds;
		int selectReturn = select(maxFd + 1, &tempReadFds, NULL, NULL, NULL);

		if(selectReturn == -1) {
			perror("select");
		}

		for (int fd = 0; fd <= maxFd; ++fd)
		{
			if(FD_ISSET(fd, &tempReadFds)) {
				if(fd == server->socketFileDescriptor) {

					struct sockaddr clientAddress;
					socklen_t clientAddressLength;

					int clientFd = accept(server->socketFileDescriptor, &clientAddress, &clientAddressLength);

					printf("> Accepted new connection %d\n", clientFd);

					FD_SET(clientFd, &readFds);

					if(clientFd > maxFd) {
						maxFd = clientFd;
					}
				} else {
					handleHttpRequest(fd);
					FD_CLR(fd, &readFds);
				}
			}
		}
	}
}
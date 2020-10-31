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
		printf("maxFd %d\n", maxFd);
		int selectReturn = select(maxFd + 1, &tempReadFds, NULL, NULL, NULL);
		printf("Select return\n");

		if(selectReturn == -1) {
			perror("select");
		}

		for (int fd = 0; fd <= maxFd; ++fd)
		{
			if(FD_ISSET(fd, &tempReadFds)) {
				printf("Evento en descriptor %d\n", fd);

				if(fd == server->socketFileDescriptor) {

					struct sockaddr clientAddress;
					socklen_t clientAddressLength;

					int clientFd = accept(server->socketFileDescriptor, &clientAddress, &clientAddressLength);

					printf("Accepteed new connection %d\n", clientFd);

					FD_SET(clientFd, &readFds);

					if(clientFd > maxFd) {
						maxFd = clientFd;
					}
				} else {

					memset(&buffer, 0, 256);
					ssize_t readed = recv(fd, &buffer, 256, 0);
					if(readed < 1) {
						FD_CLR(fd, &readFds);
						continue;
					} else {
						printf("Client %d: %s\n", fd, buffer);
					}
				}
			}
		}
	}
}
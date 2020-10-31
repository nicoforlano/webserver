cc = gcc

all: webserver clean-o

webserver: main.c server.h server.o blockingServer.h blockingServer.o nonBlockingServer.h nonBlockingServer.o
	cc -g main.c server.o blockingServer.o nonBlockingServer.o -o webserver -std=c99 -pthread

server.o: server.c server.h blockingServer.h blockingServer.o nonBlockingServer.h nonBlockingServer.o
	cc -g -c server.c -std=c99 -pthread

blockingServer.o: blockingServer.c blockingServer.h
	cc -g -c blockingServer.c -pthread

nonBlockingServer.o: nonBlockingServer.c nonBlockingServer.h
	cc -g -c nonBlockingServer.c -std=c99 -pthread

clean:
	rm -rf *o webserver *~

clean-o:
	rm *.o
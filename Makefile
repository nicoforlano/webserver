cc = gcc

all: webserver clean-o

webserver: main.c server.h server.o blockingServer.h blockingServer.o nonBlockingServer.h nonBlockingServer.o threadpool.h threadpool.o
	cc -g main.c server.o blockingServer.o nonBlockingServer.o threadpool.o -o webserver -std=c99 -pthread

server.o: server.c server.h blockingServer.h blockingServer.o nonBlockingServer.h nonBlockingServer.o
	cc -g -c server.c -std=c99 -pthread

blockingServer.o: blockingServer.c blockingServer.h threadpool.o threadpool.h
	cc -g -c blockingServer.c -pthread

threadpool.o: threadpool.c threadpool.h
	cc -g -c threadpool.c -pthread

nonBlockingServer.o: nonBlockingServer.c nonBlockingServer.h
	cc -g -c nonBlockingServer.c -std=c99 -pthread

clean:
	rm -rf *o webserver *~

clean-o:
	rm *.o
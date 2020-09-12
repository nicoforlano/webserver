cc = gcc

all: webserver clean-o

webserver: main.c server.h server.o
	cc main.c server.o -o webserver -std=c99 -pthread

server.o: server.c server.h
	cc -c server.c -std=c99 -pthread

clean:
	rm -rf *o webserver *~

clean-o:
	rm *.o
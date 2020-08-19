all: main server clean-o

main: main.c server.h server.o
	gcc main.c server.o -o webserver

server: server.c server.h
	gcc -c server.c

clean:
	rm -rf *o main server *~

clean-o:
	rm *.o
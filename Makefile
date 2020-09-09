all: main server clean-o

main: main.c server.h server.o
	gcc main.c server.o -o webserver -std=C99

server: server.c server.h
	gcc -c server.c -std=C99

clean:
	rm -rf *o main server *~

clean-o:
	rm *.o
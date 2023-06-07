all:fancy-client client server

fancy-client: fancy-client.o
	gcc -Wall -Werror -lpthread -o fancy-client fancy-client.o

fancy-client.o: fancy-client.c
	gcc -c -g fancy-client.c

client: client.o
	gcc -Wall -Werror -o client client.o

client.o: client.c
	gcc -c -g client.c

server: server.o
	gcc -Wall -Werror -lpthread -o server server.o

server.o: server.c
	gcc -c -g server.c

clean:
	rm client.o client fancy-client fancy-client.o server server.o

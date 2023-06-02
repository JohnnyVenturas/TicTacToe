all:fancy-client client

fancy-client: fancy-client.o
	gcc -Wall -Werror -lpthread -o fancy-client fancy-client.o

fancy-client.o: fancy-client.c
	gcc -c -g fancy-client.c

client: client.o
	gcc -Wall -Werror -o client client.o

client.o: client.c
	gcc -c -g client.c

clean:
	rm client.o client fancy-client fancy-client.o

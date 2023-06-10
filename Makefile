#Bogdan Budura, Cassidy Kevorkian-Mielly and Sebastian Simon worked together towards the completion of this project
all:client server

client: client.o
	gcc -Wall -Werror -o client client.o

client.o: client.c
	gcc -c -g client.c

server: server.o
	gcc -Wall -Werror -lpthread -o server server.o

server.o: server.c
	gcc -c -g server.c

clean:
	rm client.o client server server.o

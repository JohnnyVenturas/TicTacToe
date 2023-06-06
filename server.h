#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_GAMES 3
#define FIRST 1
#define SECOND 2

/*
 * play_game plays a game(thread)
 *
 */

typedef struct {
    struct sockaddr_in address;
    char *msg;
} client;

//game

int check_game_validity(int game);

//*args == game 
void play_game(void *args);





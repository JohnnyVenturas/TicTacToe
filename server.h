#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_GAMES 3

/*
 * play_game plays a game(thread)
 *
 */

typedef struct {
    struct sockaddr_in address;
    char *msg;
} client;

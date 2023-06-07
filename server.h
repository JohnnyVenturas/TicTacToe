#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define FYI 0x01
#define MYM 0x02
#define END 0x03
#define TXT 0x04
#define MOV 0x05
#define LFT 0x06
#define DEBUG 1


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

int check_game_validity(int game_num, int i, int j, int player_num);
'''
-2 illegal move
-1 normal move
0 draw
1 player 1 won
2 draw
'''

//*args == game 
void play_game(void *args);





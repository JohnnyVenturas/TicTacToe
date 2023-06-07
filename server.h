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

#define FYI 0x01
#define MYM 0x02
#define END 0x03
#define TXT 0x04
#define MOV 0x05
#define LFT 0x06
#define SPACE 0x20
#define TRUE 1
#define FALSE 0
#define FIND_GAME(client) (client / 2)
#define CLIENT_NUMBER(client) (client % 2)
/*
 * play_game plays a game(thread)
 *
 */


typedef struct {
    struct sockaddr_in address;
    char *msg;
} client;

void zero_game_boards();

//game

int check_game_validity(int game);

//*args == game 
void play_game(void *args);

int find_address(struct sockaddr_in *address);
void init_game_threads();
void send_welcome(struct sockaddr_in *address);
void debug_message(char *message);
    




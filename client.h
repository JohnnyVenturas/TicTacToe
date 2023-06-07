#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define FYI 0x01
#define MYM 0x02
#define END 0x03
#define TXT 0x04
#define MOV 0x05
#define LFT 0x06
#define DEBUG 0
/*
 * Messages From Server to Clients
 * */
/*TXT. Text message (to be displayed at the screen of the client). The message terminates with the string termination
  ('\0') character.
  +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
  | TXT    | 'W'    | 'e'    | 'l'    | 'c'    | 'o'    | 'm'    | 'e'    | '!'    | '\0'   |
  +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
 */
/*

 * MYM. Make Your Move, no parameters.
*   +--------+
    | MYM    |
    +--------+

 *END. Terminates game, and indicates winner (0 in case of draw, 1 if player 1 has won, 2 if player 2 has won). Value
255 (0xFF) is used to indicate to a new client that there is no room for new participants.

 *
 *
 *FYI. Sends filled positions in the board. n indicates the number of filled positions; for each position (from 1 to n),
the player (1 or 2), the column (0, 1, 2) and the row (0, 1, 2) are indicated.
  +--------+--------+--------+--------+--------+--- ... ---+--------+--------+--------+
  | FYI    | n      | play_1 | col_1  | row_1  |           | play_n | col_n  | row_n  |
  +--------+--------+--------+--------+--------+--- ... ---+--------+--------+--------+
 *
 *
*/
/*Messages From Client to Server
   +--------+--------+--------+
  | MOV    | col    | row    |
  +--------+--------+--------+


  */
struct read_buffer {
    char *buffer;
    int buf_length;
};

void tic_tac_toe(int sockfd, struct sockaddr_in *address);
void print_board(char *message_buffer, int size);
void get_move(int sockfd, struct sockaddr *address, char *send_buf);

void printByteByByte( void* buffer, int size);

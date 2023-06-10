//Bogdan Budura, Cassidy Kevorkian-Mielly and Sebastian Simon worked together towards the completion of this project

#include "server.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
char games[MAX_GAMES][32];
int game_update[MAX_GAMES];
int which_client_update[MAX_GAMES];
int number_clients = 0;
int sockfd;
client clients[2 * MAX_GAMES];
pthread_t game_threads[MAX_GAMES];

client clients[2 * MAX_GAMES];

pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;

// i and j are the positions of the next move to make and player_num is 1 if its player 1's move else 2

int check_game_validity(int game_num, int i, int j) {
    //printf("we are in check_game_validity \n");
    int player_num = which_client_update[game_num] + 1;

    //printf("GN == %d i == %d j == %d \n", game_num, i, j);

    char board[3][3] = {{SPACE, SPACE, SPACE}, {SPACE, SPACE, SPACE}, {SPACE, SPACE, SPACE}};

    char player_char[3] = {'\0', 'X', 'O'};

    int k;
    for (k = 2; k < 3 * games[game_num][1] + 2; k += 3) {
        board[games[game_num][k + 1]][games[game_num][k + 2]] = player_char[games[game_num][k]];
    }

    char letter = player_char[player_num];

    if (board[i][j] != SPACE) {
        return -2;
    }

    board[i][j] = letter;
    int n = games[game_num][1];
    games[game_num][1]++;

    games[game_num][3 * n + 2] = player_num;
    games[game_num][3 * n + 3] = i;
    games[game_num][3 * n + 4] = j;
    games[game_num][3 * n + 5] = '\0';
    // Check all the lines

    if ((board[0][0] == letter) && (board[0][1] == letter) && (board[0][2] == letter)) {
        return player_num;
    }

    if ((board[1][0] == letter) && (board[1][1] == letter) && (board[1][2] == letter)) {
        return player_num;
    }

    if ((board[2][0] == letter) && (board[2][1] == letter) && (board[2][2] == letter)) {
        return player_num;
    }

    // Check all the columns

    if ((board[0][0] == letter) && (board[1][0] == letter) && (board[2][0] == letter)) {
        return player_num;
    }
    if ((board[0][1] == letter) && (board[1][1] == letter) && (board[2][1] == letter)) {
        return player_num;
    }
    if ((board[0][2] == letter) && (board[1][2] == letter) && (board[2][2] == letter)) {
        return player_num;
    }

    // Check diagonal

    if ((board[0][0] == letter) && (board[1][1] == letter) && (board[2][2] == letter)) {
        return player_num;
    }

    // Check antidiagonal

    if ((board[0][2] == letter) && (board[1][1] == letter) && (board[2][0] == letter)) {
        return player_num;
    }

    // Check draw

    if (games[game_num][1] == 9) {
        return 0;
    }

    return -1;
}

void play_game(void *args) {
    int game = *((int *)args);
    while (1) {
        if (game_update[game] == 0) continue;
        // printf("%d \n", game_update[game]);

        int current_player = 2 * game + which_client_update[game];
        int other_player = 2 * game + (1 - which_client_update[game]);

        // clients[current_player].msg[2]);
        printf("Starting to receive form client mov \n");
        printByteByByte(clients[current_player].msg, 3);
        printf("Finish to receive form client mov \n");
        int game_status =
            check_game_validity(game, (int)(clients[current_player].msg[1]),
                                (int)(clients[current_player].msg[2]));  // ASLO UPDATES THE STATE OF THE GAME

        if (DEBUG == 1) {
            printf("game status is %d \n", game_status + 100);
            printf("The state of the game after update is : \n %s \n", games[game]);
        }

        int sendclient;
        if (game_status < 0) {
            client *destination;
            if (game_status == -2) {
                destination = &clients[current_player];
                sendclient = current_player;
            } else {
                destination = &clients[other_player];
                sendclient = other_player;
            }

            char send_buf = MYM;
            pthread_mutex_lock(&sock_mutex);
            printf("Sending FYI to client: %d with address %s \n", sendclient,
                   inet_ntoa(destination->address.sin_addr));

            printByteByByte(games[game], strlen(games[game]));

            sendto(sockfd, (games[game]), 3 * games[game][1] + 2, 0, (struct sockaddr *)&destination->address,
                   sizeof(struct sockaddr_in));  // send the FYI
            printf("Sending MYM to: %d with address %s \n", current_player, inet_ntoa(destination->address.sin_addr));

            sendto(sockfd, &send_buf, 1, 0, (struct sockaddr *)&destination->address,
                   sizeof(struct sockaddr_in));  // send the make you move
            pthread_mutex_unlock(&sock_mutex);
            game_update[game] = 0;
            continue;
        }

        // if we are here either the current player won

        char send_buf[2];
        send_buf[0] = END, send_buf[1] = (unsigned char)(game_status);
        pthread_mutex_lock(&sock_mutex);
        printf("Sending END to: \n");
        sendto(sockfd, (void *)(&send_buf), 2, 0, (struct sockaddr *)&clients[current_player].address,
               sizeof(struct sockaddr_in));  // send the make you move
        printf("Sending END to: \n ");
        sendto(sockfd, (void *)(&send_buf), 2, 0, (struct sockaddr *)&clients[other_player].address,
               sizeof(struct sockaddr_in));  // send the make you move

        pthread_mutex_unlock(&sock_mutex);
        game_update[game] = 0;
    }
}

client clients[2 * MAX_GAMES];

// i and j are the positions of the next move to make and player_num is 1 if its player 1's move else 2

int main(int argc, char **argv) {
    init_game_update();
    zero_game_boards();
    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "Bind failed: %s \n", strerror(errno));
    }

    printf("Server started succesfully \n");
    printf("Listening on port %s \n", argv[1]);

    struct sockaddr_in received_address;
    socklen_t size;
    char temp_buf[128];
    init_game_threads();

    while (1) {
        recvfrom(sockfd, temp_buf, 128, 0, (struct sockaddr *)&received_address, &size);
        // printf("Starting to receive form client mov XXXXXXXXX \n");
        printByteByByte(temp_buf, 20);

        // printf("Finishing to receive form client mov XXXXXXXXX \n");
        if (DEBUG) {
            debug_message(temp_buf);
        }

        int cur_client = find_address(&received_address, temp_buf);

        printf("%d \n", cur_client);
        char problem[2] = {END, 0xFF};
        if (cur_client == -1) {
             pthread_mutex_lock(&sock_mutex);
            sendto(sockfd, problem, 2, 0, (struct sockaddr *)&received_address, size);
             pthread_mutex_unlock(&sock_mutex);
        }
    }
}

void zero_game_boards() {
    int i;
    for (i = 0; i < MAX_GAMES; ++i) {
        games[i][0] = FYI;
        games[i][1] = 0;
        games[i][2] = '\0';
    }
}

void init_game_update() {
    int i;
    for (i = 0; i < MAX_GAMES; ++i) {
        game_update[i] = FALSE;
    }
}

void init_game_threads() {
    int *values = malloc(MAX_GAMES);
    int i;
    for (i = 0; i < MAX_GAMES; ++i) {
        values[i] = i;
        pthread_create(&game_threads[i], NULL, (void *)play_game, (void *)&values[i]);
    }
}

int find_address(struct sockaddr_in *address, char *temp_buf) {
    int i = 0;
    for (i = 0; i < number_clients; ++i) {
        if (clients[i].address.sin_addr.s_addr == address->sin_addr.s_addr &&
            // might need a mutex here for game_update
            clients[i].address.sin_port == address->sin_port) {
            memcpy(clients[i].msg, temp_buf, 128);
            int cur_game = FIND_GAME(i);
            which_client_update[cur_game] = CLIENT_NUMBER(i);
            game_update[cur_game] = TRUE;
            return i;
        }
    }
    if (number_clients >= MAX_GAMES * 2) {
        return -1;
    }
    number_clients++;
    clients[number_clients - 1].address = *address;
    clients[number_clients - 1].msg = malloc(128);

    send_welcome(number_clients - 1);
    int cur_game = FIND_GAME(number_clients - 1);
    if ((number_clients - 1) % 2 == 1) {
        pthread_mutex_lock(&sock_mutex);
        printf("Sending FYI to client: %d with address %s\n", number_clients - 2,
               inet_ntoa(clients[number_clients - 2].address.sin_addr));

        printByteByByte(games[cur_game], strlen(games[cur_game]));

        sendto(sockfd, (void *)(games[cur_game]), games[cur_game][1] * 3 + 2, 0,
               (struct sockaddr *)&clients[number_clients - 2].address,
               sizeof(struct sockaddr_in));  // send the FYI
        pthread_mutex_unlock(&sock_mutex);
        pthread_mutex_lock(&sock_mutex);

        printf("Sending MYM to client: %d with address %s \n", number_clients - 2,
               inet_ntoa(clients[number_clients - 2].address.sin_addr));

        char send_buf = MYM;
        sendto(sockfd, (void *)&send_buf, 1, 0, (struct sockaddr *)&clients[number_clients - 2].address,
               sizeof(struct sockaddr_in));  // send the make you move
        pthread_mutex_unlock(&sock_mutex);
    }
    return number_clients - 1;
}

void send_welcome(int clientid) {
    char buffer[64];
    buffer[0] = TXT;
    snprintf(buffer, 64, "%cWelcome you are player %d in game %d\n", TXT, CLIENT_NUMBER(clientid), FIND_GAME(clientid));
    pthread_mutex_lock(&sock_mutex);
    sendto(sockfd, buffer, 64, 0, (struct sockaddr *)&clients[clientid].address, sizeof(struct sockaddr_in));
    pthread_mutex_unlock(&sock_mutex);
}

void debug_message(char *message) {
    // char buffer[128];
    printf("Ne futem aici in pizda in debug \n");
    switch (message[0]) {
        case FYI:
            printf("AVEM UN FYI \n");
            // snprintf(buffer, 4, "%s", "FYI");
            break;
        case MYM:
            printf("AVEM UN MYM \n");
            // snprintf(buffer, 4,"%s" ,"MYM");
            break;
        case END:
            printf("AVEM UN END \n");
            // snprintf(buffer, 4,"%s" ,"END");
            break;
        case TXT:
            printf("AVEM UN TXT \n");
            // snprintf(buffer, 4,"%s" ,"TXT");
            break;
        case MOV:
            // snprintf(buffer, 4,"%s" ,"MOV");
            printf("AVEM UN MOV \n");
            break;
        case LFT:
            printf("AVEM UN LFT \n");
            // snprintf(buffer, 4,"%s" ,"LFT");
            break;
    }

    // printf("%s \n", buffer);
}

void printByteByByte(void *buffer, int size) {
    printf("Printing Byte by byte \n");
    printf("The size is %d \n", size);
    unsigned char *bytes = (unsigned char *)buffer;
    int i;

    for (i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
    }

    printf("\n");
}

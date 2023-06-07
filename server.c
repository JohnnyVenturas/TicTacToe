#include "server.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
char games[MAX_GAMES][32];
int game_update[MAX_GAMES];
int which_client_update[MAX_GAMES];
int number_clients = 0;
int sockfd;
pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;
client clients[2 * MAX_GAMES];
pthread_t game_threads[MAX_GAMES];

int DEBUG = TRUE;

int main(int argc, char **argv) {
    zero_game_boards();

    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "Bind failed: %s \n", strerror(errno));
    }

    struct sockaddr_in received_address;
    socklen_t size;
    char temp_buf[1024];
    init_game_threads();

    while (1) {
        pthread_mutex_lock(&sock_mutex);
        recvfrom(sockfd, temp_buf, 1024, 0, (struct sockaddr *)&received_address, &size);
        pthread_mutex_unlock(&sock_mutex);

        if (DEBUG) {
            debug_message(temp_buf);
        }

        int cur_client = find_address(&received_address);
        if (cur_client == -1) {
            pthread_mutex_lock(&sock_mutex);
            sendto(sockfd, "Too many games at the moment", 32, 0, (struct sockaddr *)&received_address, size);
            pthread_mutex_unlock(&sock_mutex);
        }

        int cur_game = FIND_GAME(cur_client);
        strncpy(clients[cur_client].msg, temp_buf, 128);
        which_client_update[cur_game] = CLIENT_NUMBER(cur_client);
        game_update[cur_game] = TRUE;
    }
}

void zero_game_boards() {
    int i;
    for (i = 0; i < MAX_GAMES; ++i) {
        games[i][0] = FYI;
    }
}

void init_game_threads() {
    int i;
    for (i = 0; i < MAX_GAMES; ++i) {
        pthread_create(&game_threads[i], NULL, (void *)play_game, (void *)&i);
    }
}

int find_address(struct sockaddr_in *address) {
    int i = 0;
    for (i = 0; i < number_clients; ++i) {
        if (clients[i].address.sin_addr.s_addr == address->sin_addr.s_addr &&
            clients[i].address.sin_port == address->sin_port) {
            return i;
        }
    }
    if (number_clients * 2 == MAX_GAMES) {
        return -1;
    }
    number_clients++;
    clients[number_clients].address = *address;
    clients[number_clients].msg = malloc(128);
    send_welcome(&clients[number_clients].address);
    return number_clients;
}

void send_welcome(struct sockaddr_in *address) {
    char buffer[32];
    buffer[0] = TXT;
    strncpy(&buffer[1], "Welcome", 31);
    pthread_mutex_lock(&sock_mutex);
    sendto(sockfd, buffer, 32, 0, (struct sockaddr *)address, sizeof(struct sockaddr_in));
    pthread_mutex_lock(&sock_mutex);
}

void debug_message(char *message) {
    char buffer[128];
    switch (message[0]) {
        case FYI:
            snprintf(buffer, 4, "FYI");
            break;
        case MYM:
            snprintf(buffer, 4, "MYM");
            break;
        case END:
            snprintf(buffer, 4, "END");
            break;
        case TXT:
            snprintf(buffer, 4, "TXT");
            break;
        case MOV:
            snprintf(buffer, 4, "MOV");
            break;
        case LFT:
            snprintf(buffer, 4, "LFT");
            break;
    }
    snprintf(buffer + 3, 125, "%s", message);

    printf("%s \n", message);
}

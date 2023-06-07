#include "server.h"
#include <sys/socket.h>
char games[MAX_GAMES][32];
int game_update[MAX_GAMES];
int which_client_update[MAX_GAMES];
int number_clients = 0;

client clients[2 * MAX_GAMES];

pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;

// i and j are the positions of the next move to make and player_num is 1 if its player 1's move else 2

int check_game_validity(int game_num, int i, int j)
{
    int player_num = which_client_update[game_num] + 1;
    char board[3][3] = {
        {SPACE, SPACE, SPACE},
        {SPACE, SPACE, SPACE},
        {SPACE, SPACE, SPACE}};

    char player_char[3] = {'\0', 'X', 'O'};

    for (int k = 2; k < 3 * games[game_num][1] + 2; k += 3)
    {
        board[k + 1][k + 2] = player_char[k];
    }

    char letter = player_char[player_num];

    if (board[i][j] != SPACE)
    {
        return -2;
    }

    board[i][j] = letter;
    int n = games[game_num][1];
    games[game_num][1]++;

    games[game_num][3 * n + 2] = player_num;
    games[game_num][3 * n + 3] = i;
    games[game_num][3 * n + 4] = j;
    // Check all the lines

    if ((board[0][0] == letter) && (board[0][1] == letter) && (board[0][2] == letter))
    {
        return player_num;
    }

    if ((board[1][0] == letter) && (board[1][1] == letter) && (board[1][2] == letter))
    {
        return player_num;
    }

    if ((board[2][0] == letter) && (board[2][1] == letter) && (board[2][2] == letter))
    {
        return player_num;
    }

    // Check all the columns

    if ((board[0][0] == letter) && (board[1][0] == letter) && (board[2][0] == letter))
    {
        return player_num;
    }
    if ((board[0][1] == letter) && (board[1][1] == letter) && (board[2][1] == letter))
    {
        return player_num;
    }
    if ((board[0][2] == letter) && (board[1][2] == letter) && (board[2][2] == letter))
    {
        return player_num;
    }

    // Check diagonal

    if ((board[0][0] == letter) && (board[1][1] == letter) && (board[2][2] == letter))
    {
        return player_num;
    }

    // Check antidiagonal

    if ((board[0][2] == letter) && (board[1][1] == letter) && (board[2][0] == letter))
    {
        return player_num;
    }

    // Check draw

    if (games[game_num][1] == 9)
    {

        return 0;
    }

    return -1;
}

void play_game(void *args)
{
    int game = *((int *)arg);

    while (1)
    {

        if (game_update[game] == 0)
            continue;

        int current_player = 2 * game + which_client_update[i];
        int other_player = 2 * game + (1 - which_client_update[i]);

        int game_status = check_game_validity(game, int(clients[game].msg[1]), int(clients[game].msg[2])); // ASLO UPDATES THE STATE OF THE GAME

        if (DEBUG == 1)
        {
            printf("game status is %d \n", game_status);
            printf("The state of the game after update is : \n %s \n", games[game]);
        }

        if (game_status < 0)
        {
            client *destination;
            if (game_status == -2)
                destination = &clients[current_player];
            else
                destination = &clients[other_player];

            char send_buf = MYM;
            pthread_mutex_lock(&sock_mutex);
            printf("Sending FYI to: \n");
            sendto(sockfd, void *(games[game])), strlen(games[game]), 0,
                (struct sockaddr *)&destination->address, sizeof(struct sockaddr_in); // send the FYI
            printf("Sending MYM to:");

            sendto(sockfd, void *(&sendbuf), 1, 0,
                   (struct sockaddr *)&destination->address, sizeof(struct sockaddr_in)); // send the make you move
            pthread_mutex_unlock(&sock_mutex);
            game_update[game] = 0;
            free(send_buf);
            continue;
        }

        // if we are here either the current player won

        char send_buf[2];
        send_buf[0] = END, send_buf[1] = (unsigned char)(game_status);
        pthread_mutex_lock(&sock_mutex);
        printf("Sending END to:");
        sendto(sockfd, void *(&sendbuf), 2, 0,
               (struct sockaddr *)&clients[current_player].address, sizeof(struct sockaddr_in)); // send the make you move
        printf("Sending END to:");
        sendto(sockfd, void *(&sendbuf), 2, 0,
               (struct sockaddr *)&clients[other_player].address, sizeof(struct sockaddr_in)); // send the make you move

        pthread_mutex_unlock(&sock_mutex);
        free(send_buf);
        game_update[game] = 0;
    }
}

int main(int argc, char **argv)
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        fprintf(stderr, "Bind failed: %s \n", strerror(errno));
    }

    struct sockaddr_in recieved_address;
    socklen_t size;
    char temp_buf[1024];

    recvfrom(sockfd, temp_buf, 1024, 0, &recieved_address, &size);
}

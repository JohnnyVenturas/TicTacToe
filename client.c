//Bogdan Budura, Cassidy Kevorkian-Mielly and Sebastian Simon worked together towards the completion of this project
#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
int main(int argc, char **argv) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_port = htons(atoi(argv[2]));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &address.sin_addr);

    tic_tac_toe(sockfd, &address);
}

void tic_tac_toe(int sockfd, struct sockaddr_in *address) {
    char init_buf[10] = "Hello";
    sendto(sockfd, init_buf, strlen(init_buf), 0, (struct sockaddr *)address, sizeof(struct sockaddr_in));
    char *message_buffer = malloc(1024);
    memset(message_buffer, 0, 1024);

    struct sockaddr_in recv_address;
    socklen_t recv_size;
    char send_buf[32];
    int recv_bytes;
    int to_send;
    while (1) {
        if ((recv_bytes = recvfrom(sockfd, message_buffer, 1024, 0, (struct sockaddr *)&recv_address, &recv_size)) ==
            -1) {
            fprintf(stderr, "Error in receiving bytes form server");
            exit(1);  // or maybe make the function in and return 1
        }
        to_send = 1;

        message_buffer[recv_bytes] = '\0';

        if (DEBUG) {
            printf("DEBUG: byte by byte from buffer: \n");
            printByteByByte(message_buffer, recv_bytes);
        }
        switch (message_buffer[0]) {
            case FYI:
                print_board(message_buffer, recv_bytes);
                break;
            case TXT:
                printf("%s \n", message_buffer);  // inainte era +1 nush de ce
                break;
            case MYM:
                get_move(sockfd, (struct sockaddr *)address, send_buf);
                break;

            case END:
                if (message_buffer[1] == 1) {
                    printf("Player One won \n");
                } else if (message_buffer[1] == 2) {
                    printf("Player Two won \n");
                } else if (message_buffer[1] == 0) {
                    printf("\nDRAW\n");
                } else {
                    printf("\nNo room for you\n");
                }

                exit(1);
        }
    }
}

void print_board(char *message_buffer, int size) {
    //printf("We are in clinet in FYI \n");
    //printByteByByte(message_buffer, size);
    if (*message_buffer != FYI) {
        fprintf(stderr, "We have encountered and error, not FYI \n");
    }

    int filled_positions = *(message_buffer + 1);
    char matrix[3][3];
    memset(matrix, 0, sizeof(matrix));
    int i, j, k;

    for (i = 2; i < size; i += 3) {
        j = message_buffer[i + 1];
        k = message_buffer[i + 2];

        if (j > 2 || j < 0) {
            fprintf(stderr, "We have an error:Bad Index \n");
            // exit(-1);
        }

        if (k > 2 || k < 0) {
            fprintf(stderr, "We have an error:Bad Index \n");
            // exit(-1);
        }

        matrix[j][k] = message_buffer[i] == 1 ? 'X' : 'O';
    }
    //printf("We are in clinet in FYI. Here we should print the empty matrix \n");
    //printByteByByte(message_buffer, size);

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            if (matrix[i][j] == 0) printf("_");
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void get_move(int sockfd, struct sockaddr *address, char *send_buf) {
    while (1) {
        printf("Please make your move by entering row and column 0,1 or 2: \n");

        if (fgets(send_buf, 32, stdin) == NULL) {
            fprintf(stderr, "An error has occured while reading stdin %s \n", strerror(errno));
            exit(1);
        }

        int row, col;
        //printf("\n send_buf is : %s", send_buf);
        sscanf(send_buf, "%d %d", &row, &col);

        if (row < 0 || row > 2 || col < 0 || col > 2) {
            printf("Your move was not valid! \n");
            continue;
        }
        //printf("We are printing send_buf byte by byte: \n");
        //printByteByByte(send_buf, 3);
        send_buf[0] = MOV;
        send_buf[1] = (unsigned char)row;
        send_buf[2] = (unsigned char)col;

        if (DEBUG == 1) {
            printf("\n DEBUG: row is: %d, col is %d \n", row, col);
            printf("We are printing send_buf byte by byte: \n");
            printByteByByte(send_buf, 3);
        }

        sendto(sockfd, send_buf, 3, 0, (struct sockaddr *)address, sizeof(struct sockaddr_in));
        break;
    }
}

void printByteByByte(void *buffer, int size) {
    unsigned char *bytes = (unsigned char *)buffer;
    int i;

    for (i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
    }

    printf("\n");
}

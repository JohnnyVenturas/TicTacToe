
#include "client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
int main(int argc, char **argv) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_port = htons(atoi(argv[2]));
    inet_pton(sockfd, argv[1], &address.sin_addr);
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
    while (1) {
        if ((recv_bytes = recvfrom(sockfd, message_buffer, 1024, 0, (struct sockaddr *)&recv_address, &recv_size)) ==
            -1) {
            fprintf(stderr, "Error in receiving bytes form server");
            exit(1);  // or maybe make the function in and return 1
        }
        if (DEBUG) {
            printf("DEBUG: byte by byte from buffer: \n");
            printByteByByte(message_buffer, recv_bytes);
        }
        switch (message_buffer[0]) {
            case FYI:
                break;
            case TXT:
                printf("%s \n", message_buffer);  // inainte era +1 nush de ce
                break;
            case MYM:
                get_move(sockfd, (struct sockaddr *)address, send_buf);
                break;

            case END:
                if (strlen(message_buffer) > 1) {
                    printf("%s cplm \n", message_buffer + 1);
                    exit(1);
                }
                printf("\n DRAW \n ");
                exit(1);
        }
    }
}

void print_board(char *message_buffer) { printf("%s \n", message_buffer); }

void get_move(int sockfd, struct sockaddr *address, char *send_buf) {
    printf("Please make your move by entering row and column: \n");

    if (fgets(send_buf, 32, stdin) == NULL) {
        fprintf(stderr, "An error has occured while reading stdin %s \n");
        exit(1);
    }

    int row, col;
    printf("\n send_buf is : %s", send_buf);
    sscanf(send_buf, "%d %d", &row, &col);
    printf("We are printing send_buf byte by byte: \n");
    printByteByByte(send_buf, 3);
    send_buf[0] = MOV;
    send_buf[1] = (unsigned char)row;
    send_buf[2] = (unsigned char)col;

    if (DEBUG == 1) {
        printf("\n DEBUG: row is: %d, col is %d \n", row, col);
        printf("We are printing send_buf byte by byte: \n");
        printByteByByte(send_buf, 3);
    }
    sendto(sockfd, send_buf, 3, 0, (struct sockaddr *)address, sizeof(struct sockaddr_in));
}

void printByteByByte(void *buffer, int size) {
    unsigned char *bytes = (unsigned char *)buffer;
    int i;
    for (i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

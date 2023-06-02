
#include "client.h"

#include <arpa/inet.h>
#include <netinet/in.h>
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
    while (1) {
        recvfrom(sockfd, message_buffer, 1024, 0, (struct sockaddr *)&recv_address, &recv_size);
        printf("We received from server message buffer %s \n", message_buffer);
        switch (message_buffer[0]) {
            case FYI:
                break;
            case TXT:
                printf("%s \n", message_buffer + 1);
                break;
            case MYM:
                get_move(sockfd, (struct sockaddr *)address, send_buf);
                break;

            case END:
                if (strlen(message_buffer) > 1) printf("%s \n", message_buffer + 1);
                exit(1);
        }
    }
}

void print_board(char *message_buffer) { printf("%s \n", message_buffer); }
void get_move(int sockfd, struct sockaddr *address, char *send_buf) {}


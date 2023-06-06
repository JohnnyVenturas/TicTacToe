#include "server.h"

int main(int argc, char **argv) {
    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(bind(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "Bind failed: %s \n", strerror(errno));
    }
     

}




#include "server.h"
#include <sys/socket.h>
int games[MAX_GAMES][3][3];
int game_update[MAX_GAMES];
int which_client_update[MAX_GAMES];
int number_clients = 0;

client clients[2*MAX_GAMES];

int main(int argc, char **argv) {
    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(bind(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "Bind failed: %s \n", strerror(errno));
    }
    
    struct sockaddr_in recieved_address;
    socklen_t size;
    char temp_buf[1024];

    recvfrom(sockfd,temp_buf, 1024, 0, &recieved_address, &size);
      


     

}




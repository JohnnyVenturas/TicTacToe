#include <stdio.h>
#include "server.h"
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    
    struct sockaddr_in address;
    struct sockaddr_in *ptr = &address;
    memset(&address, 0, sizeof(struct sockaddr_in));

    address.sin_family = AF_INET;
    address.sin_port = htons(7000);

    struct sockaddr_in second_adresss = *ptr;
    printf("%d == %d ?? \n", address.sin_port, second_adresss.sin_port);
}

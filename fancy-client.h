#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

typedef struct {
    char *buffer;
    int max_size;

} read_buffer;

struct recvfrom_args {
    int sockfd;
    read_buffer *buffer;
    int flags;
    struct sockaddr *addr;
    socklen_t *addr_len;
};

struct sendlineto_args {
    read_buffer *buffer;
    int sockfd;
    int flags;
    struct sockaddr *addr;
    socklen_t addr_len;
};

int smart_send(int socket, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len,
               int chunk_size);
void *recvfrom_threaded(void *args);
void* read_line_send(void *args);

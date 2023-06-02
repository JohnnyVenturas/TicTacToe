#include "fancy-client.h"

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
 //pthread_mutex_t mutex_send = PTHREAD_MUTEX_INITIALIZER;
 //pthread_mutex_t mutex_recv = PTHREAD_MUTEX_INITIALIZER;
//Bogdan Budura and Sebastian Simon worked equally for this project and we have the same source code

int main(int argc, char **argv) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_port = htons(atoi(argv[2]));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &address.sin_addr);

    read_buffer tosend_buffer;
    tosend_buffer.buffer = malloc(2048);
    tosend_buffer.max_size = 2048;

    read_buffer torecv_buffer;
    torecv_buffer.buffer = malloc(2048);
    torecv_buffer.max_size = 2048;

    pthread_t send_line_thread, recv_line_thread;
    struct recvfrom_args recv_args;
    struct sendlineto_args send_args;

    send_args.sockfd = sockfd;
    send_args.buffer = &tosend_buffer;
    send_args.addr_len = sizeof(struct sockaddr_in);
    send_args.addr = (struct sockaddr *)&address;
    pthread_create(&send_line_thread, NULL, read_line_send, (void *)&send_args);
    recv_args.sockfd = sockfd;
    recv_args.buffer = &torecv_buffer;
    recv_args.flags = 0;
    recv_args.addr = NULL;
    recv_args.addr_len = NULL;

    pthread_create(&recv_line_thread, NULL, recvfrom_threaded, (void *)&recv_args);
    pthread_join(recv_line_thread, NULL);
    pthread_join(send_line_thread, NULL);

    return 0;
}

void *read_line_send(void *args) {
#define EXIT "\n\n"
    // We return -1 on failiure and sizeof the read string on return.
    //  We assuume the initial max size is a multiple of 1024 for safety
    // buf_start will hold the offset from the first element in the string
    // effectively giving us the current start of the string At the end of the
    // function buf_start will contain the size of the string by construction

    //pthread_mutex_lock(&mutex_send);
    struct sendlineto_args send_args = *(struct sendlineto_args *)args;
    read_buffer *buffer = send_args.buffer;
    int buf_start = 0;

    while (1) {
        buf_start = 0;
        while (1) {
            if (fgets(buffer->buffer + buf_start, 1024, stdin) == NULL) {
                fprintf(stderr, "An error has occured %s \n", strerror(errno));
                return NULL;
            }
            buf_start += strlen(buffer->buffer + buf_start);
            if (buf_start + 1024 >= buffer->max_size) {
                buffer->buffer = realloc(buffer->buffer, buf_start * 2);
                buffer->max_size = buf_start * 2;
            }

            if (strncmp(buffer->buffer + buf_start - 2, EXIT, 2) == 0) {
                *(buffer->buffer + buf_start - 2) = '\0';
                //printf("Ce pula mea are asta %s \n", buffer->buffer);
                break;
            }
        }

        int bytes_sent, size = buf_start -1;

        if ((bytes_sent = sendto(send_args.sockfd, buffer->buffer, size, 0, send_args.addr, send_args.addr_len) == -1)) {
            fprintf(stderr, "We have encountered an error %s \n", strerror(errno));
        }

        //pthread_mutex_unlock(&mutex_send);
    }
    return NULL;
}

void *recvfrom_threaded(void *args) {
    struct recvfrom_args recv_args = *(struct recvfrom_args *)args;
    int recv_bytes;
    //pthread_mutex_lock(&mutex_recv);

    while (1) {
        memset(recv_args.buffer->buffer, 0, recv_args.buffer->max_size);
        if ((recv_bytes = recvfrom(recv_args.sockfd, recv_args.buffer->buffer, recv_args.buffer->max_size, recv_args.flags, recv_args.addr, recv_args.addr_len)) == -1) {
            fprintf(stderr, "We have encountered an error while recieving a message %s\n", strerror(errno));
        }
        printf("We received the message:\n%s\n", (char *)recv_args.buffer->buffer);
    }
    //pthread_mutex_unlock(&mutex_recv);

    return NULL;
}

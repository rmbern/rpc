#include <sys/types.h> // AF_LOCAL, SOCK_STREAM, sockaddr
#include <sys/socket.h> // connect
#include <sys/un.h> // sockaddr_un
#include <unistd.h> // write
#include <stdlib.h> // exit
#include <stdio.h> // perror
#include <netdb.h> // getaddrinfo, freeaddrinfo
#include "common.h" // handle_getaddr_error

typedef enum direction {LEFT,RIGHT} direction;

char * rotate(char * rotate_me, direction dir)
{
    static int sd = -2; // -1 reserved for sys error
    if (sd == -2)
    {
        struct addrinfo request;
        memset(&request, 0, sizeof(struct sockaddr));
        request.ai_flags = 0;
        request.ai_family = AF_INET;
        request.ai_socktype = SOCK_STREAM;
        request.ai_protocol = 0;
        request.ai_addrlen = 0;
        request.ai_canonname = NULL;
        request.ai_next = NULL;

        struct addrinfo * responses;

        int getaddr_result = getaddrinfo("localhost", 
                                         "5000", 
                                         &request,
                                         &responses);
        handle_getaddr_error(getaddr_result);

        sd = socket(AF_INET, // Will use the network later
                    SOCK_STREAM, // TCP
                    0); // Use configured reasonable defaults 

        if (sd == -1)
        {
            perror("Socket creation\n");
            exit(1);
        }

        struct sockaddr address;
        memset(&address, 0, sizeof(struct sockaddr));

        if (connect(sd,
                    responses->ai_addr, 
                    sizeof(struct sockaddr)) == -1)
        {
            perror("Socket connect");
            exit(1);
        }
        
    }
    char direction_byte = -1;
    if (dir == LEFT)
    {
        direction_byte = 'L';
    }
    else
    {
        direction_byte = 'R';
    }
    if (write(sd, &direction_byte, 1) == -1)
    {
        perror("Socket write");
        exit(1);
    }

    if (write(sd, rotate_me, 100) == -1)
    {
        perror("Socket write");
        exit(1);
    }

    char msg_buffer[100];
    memset(msg_buffer, 0, sizeof(msg_buffer));

    if (read(sd, msg_buffer, 100) == -1)
    {
        perror("Socket read");
        exit(1);
    }
    close(sd);
    printf("%s\n", msg_buffer); 
}

int main () {
    rotate("TESTING", RIGHT);
}

#include <sys/types.h> // AF_LOCAL, SOCK_STREAM, sockaddr
#include <sys/socket.h> // connect
#include <sys/un.h> // sockaddr_un
#include <unistd.h> // write
#include <stdlib.h> // exit
#include <stdio.h> // perror
#include <netdb.h> // getaddrinfo, freeaddrinfo
#include "common.h" // handle_getaddr_error
int main () 
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

    int sd = socket(AF_INET, // Will use the network later
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
    
    if (write(sd,
          "HELLO WORLD. I AM LESS THAN 100 CHARS SO I DON'T BLOW THINGS UP\n",
          100) == -1)
    {
        perror("Socket write");
        exit(1);
    }
}

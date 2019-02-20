#include <sys/types.h> // AF_LOCAL, SOCK_STREAM, sockaddr, EAI_*, socklen_t,
                       // SOL_SOCKET, SO_REUSEADDR
#include <sys/socket.h> // socket, bind, accept, listen
#include <sys/un.h> // sockaddr_un
#include <stdio.h> // perror, printf
#include <stdlib.h> // exit
#include <string.h> // strncpy
#include <unistd.h> // read, sleep
#include <netdb.h> // getaddrinfo, freeaddrinfo
#include <pthread.h> // pthread_detach, pthread_create
                     // pthread_exit, pthread_self
#include "common.h" // handle_getaddr_error

void server_rotate(char * rotate_me, char dir)
{
    // TODO: Error handling

    // Acutal rotation done here!
    // I am refraining from using enums because I don't know
    // what it means to send an enum over a sock connection.
    
    if (dir == 'L') // left rotate
    {
        for (int i = 0; rotate_me[i+1] != 0; i++)
        {
            rotate_me[i] = rotate_me[i] ^ rotate_me[i+1];
            rotate_me[i+1] = rotate_me[i+1] ^ rotate_me[i];
            rotate_me[i] = rotate_me[i] ^ rotate_me[i+1];
        }
    }
    else // right rotate
    {
        int l; // called l because we start with the last char of the string
        for (l = 0; rotate_me[l+1] != 0; l++); 
        // l now points to the last char in rotate_me
        
        for (; l > 1; l--)
        {
            rotate_me[l] = rotate_me[l] ^ rotate_me[l-1];
            rotate_me[l-1] = rotate_me[l-1] ^ rotate_me[l];
            rotate_me[l] = rotate_me[l] ^ rotate_me[l-1];
        }
    }
}

void * connection_thread(void * args)
{
    pthread_detach(pthread_self());

    int sd = *(int*)args;
    free(args); 

    char recieved_msg[100];
    // TODO: SANITIZE INPUTS AND CLOSE ON TIMEOUT
    while(1)
    {
        memset(recieved_msg, 0, sizeof(recieved_msg));
        char direction_byte = -1;
        int result = read(sd, &direction_byte, 1);
        if (result == 0)
        {
            printf("EOF detected. Connection thread exiting\n");
            break; // Go to thread cleanup
        }
        else if (result < 0)
        {
            perror("First socket read\n");
            exit(1);
        } 

        result = read(sd, recieved_msg, sizeof(recieved_msg));
        if (result == 0)
        {
            printf("EOF detected. Connection thread exiting\n");
            break; // Go to thread cleanup
        }

        else if (result < 0)
        {
            perror("Second socket read\n");
            exit(1);
        }

        server_rotate(recieved_msg, direction_byte);

        if (write(sd, recieved_msg, sizeof(recieved_msg)) < 0)
        {
            perror("Socket write\n");
            exit(1);
        }
        printf("%s\n", recieved_msg); 
    }

    close(sd);
    pthread_exit(0);
}

int main () 
{
    struct addrinfo request;
    memset(&request, 0, sizeof(struct sockaddr));
    request.ai_flags = AI_PASSIVE;
    request.ai_family = AF_INET;
    request.ai_socktype = SOCK_STREAM;
    request.ai_protocol = 0;
    request.ai_addrlen = 0;
    request.ai_canonname = NULL;
    request.ai_next = NULL;

    struct addrinfo * responses;

    int getaddr_result = getaddrinfo(NULL, 
                                     "5000", 
                                     &request,
                                     &responses);
    handle_getaddr_error(getaddr_result);

    int sd = socket(AF_INET, 
                    SOCK_STREAM, // TCP
                    0); // Use configured reasonable defaults 

    if (sd == -1)
    {
        perror("Socket creation\n");
        exit(1);
    }
    int on = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == - 1)
    {
        perror("Socket configure\n");
        exit(1);
    }

    struct sockaddr address;
    memset(&address, 0, sizeof(struct sockaddr));

    if (bind(sd,
             responses->ai_addr, 
             sizeof(struct sockaddr)) == -1)
    {
        perror("Socket bind\n");
        exit(1);
    }

    if (listen(sd, 1) == -1)
    {
        perror("Socket listen\n");
        exit(1);
    }

    // Accept connections continuously
    while (1)
    {
        int sock_connection;
        socklen_t * socklen = malloc(sizeof(socklen_t));
        *socklen = responses->ai_addrlen;
        sock_connection = accept(sd, responses->ai_addr, socklen);
        if (sock_connection < 0)
        {
            perror("Socket accept");
            exit(1);
        }
        free(socklen); 

        int * args = malloc(sizeof(int));
        *args = sock_connection;
        pthread_t thread; // Not interested in thread id's so a single
                          // pthread_t struct will suffice.
        if (pthread_create(&thread, 0, connection_thread, args) == -1)
        {
           perror("connection thread spawn\n"); 
           exit(1);
        }
    }

    freeaddrinfo(responses);
    pthread_exit(0);
}

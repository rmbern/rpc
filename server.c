#include <sys/types.h> // AF_LOCAL, SOCK_STREAM, sockaddr
#include <sys/socket.h> // socket, bind, accept, listen
#include <sys/un.h> // sockaddr_un
#include <stdio.h> // perror, printf
#include <stdlib.h> // exit
#include <string.h> // strncpy
#include <unistd.h> // read

int main () 
{
    int sd = socket(AF_LOCAL, // Will use the network later
                    SOCK_STREAM, // TCP
                    0); // Use configured reasonable defaults 

    if (sd == -1)
    {
        perror("Socket creation\n");
        exit(1);
    }

    // TODO: BIND, LISTEN, ACCEPT
    
    struct sockaddr_un sock_path;
    memset(&sock_path, 0, sizeof(struct sockaddr_un));
    sock_path.sun_family = AF_UNIX; // man page specifies this value.

    // write "reasonable default" filehandle in process directory 
    // very not portable :)
    // We are also using a short name because of our cast below.
    // struct sockaddr only has a 14 byte buffer for an addr name
    // as opposed to the 108 byte buffer seen in struct sockaddr_un
    if (strncpy(sock_path.sun_path, "./RPC", 6) == NULL)
    {
        // TODO: Write to stderr
        printf("Error! Assignment of string for socket name failed!");
        exit(1);
    }
    // BE MINDFULL!!!!
    // Since everything is a file in UNIX(tm), whatever user running this server
    // can delete any bound file sockets with rm, or some other utility.                                          
    if (bind(sd, (struct sockaddr *)&sock_path, sizeof(struct sockaddr_un)) == -1)
    {
        perror("Socket bind\n");
        exit(1);
    }

    if (listen(sd, 1) == -1)
    {
        perror("Socket listen\n");
        exit(1);
    }

    // Accept one connection
    int accepted_socket;
    int * socklen = malloc(sizeof(int));
    *socklen = sizeof(sock_path.sun_path);
    accepted_socket = accept(sd, (struct sockaddr *)&sock_path, socklen);
    if (accepted_socket < 0)
    {
        perror("Socket accept");
        exit(1);
    }
    free(socklen);
    
    char recieved_msg[100];
    memset(recieved_msg, 0, sizeof(recieved_msg));
    
    if (read(accepted_socket, recieved_msg, sizeof(recieved_msg)) == -1)
    {
        perror("Socket read\n");
        exit(1);
    }
    printf("%s", recieved_msg); 
}

#include <sys/types.h> // AF_LOCAL, SOCK_STREAM
#include <sys/socket.h> // socket
#include <stdio.h> // perror
#include <stdlib.h> // exit
int main () 
{
    int sd = socket(AF_LOCAL, // Will use the network later
                    SOCK_STREAM, // TCP
                    0); // Use configured reasonable defaults 

    // TODO: BIND, ACCEPT, LISTEN

    if (sd == -1)
    {
        perror("Socket creation\n");
        exit(1);
    }
        
}

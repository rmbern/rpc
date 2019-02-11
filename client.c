#include <sys/types.h> // AF_LOCAL, SOCK_STREAM, sockaddr
#include <sys/socket.h> // connect
#include <sys/un.h> // sockaddr_un
#include <unistd.h> // write
#include <stdlib.h> // exit
#include <stdio.h> // perror

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

    if (connect(sd,
            (struct sockaddr *)&sock_path,
             sizeof(struct sockaddr_un)) == -1)
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

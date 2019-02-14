#include <netdb.h> 
#include <sys/types.h> // EAI_*
#include <sys/socket.h>
#include <stdio.h> // perror, printf
#include <stdlib.h> // exit

void handle_getaddr_error(int getaddr_result)
{
    if (getaddr_result != 0)
    {
        // TODO: print to stderr
        printf("getaddrinfo error!\n");
        switch (getaddr_result)
        {
            // This enum is in the getaddrinfo man page, but isn't in the
            // actual source library for some reason.
            //case EAI_ADDRFAMILY:
                //printf("Invalid address.\n");
                //printf("Code: EAI_ADDRFAMILY\n");
                //break;
            case EAI_AGAIN:
                printf("Try again later.\n");
                printf("Code: EAI_AGAIN\n");
                break;
            case EAI_BADFLAGS:
                printf("Invalid request.\n");
                printf("Code: EAI_BADFLAGS\n");
                break;
            case EAI_FAIL:
                printf("Name server returned permanent failure\n");
                printf("Code: EAI_FAIL\n");
                break;
            case EAI_FAMILY:
                printf("Request address family not supported\n");
                printf("Code: EAI_FAMILY\n");
                break;
            case EAI_MEMORY:
                printf("Out of memory\n");
                printf("Code: EAI_MEMORY\n");
                break;
            case EAI_SERVICE:
                printf("This program's sock can't handle resolve service\n");
                printf("Code: EAI_SERVICE\n");
                break;
            case EAI_SOCKTYPE:
                printf("This program's socket type not supported\n");
                printf("Code: EAI_SOCKTYPE\n");
                break;
            case EAI_SYSTEM:
                perror("getaddrinfo system error\n");
                break;
            default:
                printf("Unspecified error\n");
                break;
        }
        printf("If you recieved an EAI error code, refer to the getaddrinfo\n");
        printf("man page for more info.\n");
        exit(1);
    }
}

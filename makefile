CC=gcc
CFLAGS=-g
all:
	$(CC) $(CFLAGS) -o client client.c common.c	
	$(CC) $(CFLAGS) -o server server.c common.c	
server:
	$(CC) $(CFLAGS) -o server server.c common.c	
client:
	$(CC) $(CFLAGS) -o client client.c common.c	

CC = gcc

CFLAGS = -Wall -Wextra -g -Iinclude -Ilib
LIBS = lib/snet.a -lncurses

SERVER_OBJS = server.o session_man.o
CLIENT_OBJS = client.o session.o

all: server client

server: $(SERVER_OBJS)
	$(CC) -o $@ $^ $(LIBS)

client: $(CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIBS)

server.o: server.c session_man.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c session.h
	$(CC) $(CFLAGS) -c client.c

session_man.o: session_man.c session_man.h
	$(CC) $(CFLAGS) -c session_man.c
	
session.o: session.c session.h
	$(CC) $(CFLAGS) -c session.c

clean:
	rm -f *.o server client

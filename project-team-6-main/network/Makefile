# Makefile for CS50 Network Module

LIBS = ../libcs50/libcs50-given.a 
OBJS = client.o server.o network.o
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50
CC = gcc

all: client server

client: client.o network.o 
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

server: server.o network.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

network.o: network.h

.PHONY: all clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	rm -f client
	rm -f core

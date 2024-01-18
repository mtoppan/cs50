# Makefile for 'dealer' module
#
# Julia Gottschalk, November 2022
# CS 50, Fall 2022

OBJS = dealer.o ../cards/cards.o ../network/network.o
OBJS_test = dealertest.o ../cards/cards.o ../network/network.o
LLIBS = ../libcs50/libcs50-given.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../libcs50 -I../cards -I../network
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

dealer: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

dealertest: $(OBJS_test) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

dealer.o: ../libcs50/mem.h ../cards/cards.h ../network/network.h
dealertest.o: ../libcs50/mem.h ../cards/cards.h ../network/network.h
../cards/cards.o: ../cards/cards.h ../libcs50/mem.h ../libcs50/counters.h ../libcs50/bag.h ../libcs50/hashtable.h
../network/network.o: ../network/network.h ../cards/cards.h

.PHONY: all clean

all: dealertest dealer

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f dealer
	rm -f dealertest
	rm -f core

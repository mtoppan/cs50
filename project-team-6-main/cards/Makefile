# Makefile for 'cards' module
#
# Julia Gottschalk, November 2022
# CS 50, Fall 2022

OBJS = cardstest.o cards.o
LLIBS = ../libcs50/libcs50-given.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../libcs50
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

cardstest: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

cardstest.o: cards.h
cards.o: cards.h ../libcs50/mem.h ../libcs50/counters.h ../libcs50/bag.h ../libcs50/hashtable.h 

.PHONY: test valgrind clean

#to test for memory
valgrind: cardstest
	$(VALGRIND) ./cardstest

#to remove files produces
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f cardstest
	rm -f core

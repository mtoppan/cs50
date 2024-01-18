OBJS = player.o ../cards/cards.o ../network/network.o
LIBS =
LLIBS = ../libcs50/libcs50-given.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../libcs50 -I../cards -I../network
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

player: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

player.o: ../libcs50/mem.h ../libcs50/bag.h ../cards/cards.h
../cards/cards.o: ../cards/cards.h ../libcs50/mem.h ../libcs50/counters.h ../libcs50/bag.h ../libcs50/hashtable.h
../network/network.o: ../cards/cards.h ../network/network.h

.PHONY: clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f player
	rm -f core

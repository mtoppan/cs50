# CS50 BlackJack

### Cards

A `deck` has an ID (an integer from 0->51)for each card in the deck and a count of how many times that card has been seen (1=unseen, 2=seen). Additionally, it keeps two `hashtables`. One maps the card ID to the name of the card, the other does the opposite.
There is also a `hand`, which tracks a player or dealers hand. It maintains the name of the hand ("Player" or "Dealer"), the number of aces in their hand, the count of their hand(not including aces), and a `bag` that holds the names of the cards in the hand.

### Usage

The *cards* module, defined in `cards.h` and implemented in `cards.c`, creates a deck struct holding a `counter` and two `hashtables`. It also creates a hand struct holding `bag` of the cards it holds, two `ints` (one for the number of aces and the other for the count of the hand), and a `char*` (for the name of the hand). It exports the following functions:

```c
deck_t *initializeGame();
void deleteDeck(deck_t *deck);
hand_t *initializeDealer(deck_t *deck);
hand_t *initializePlayer(deck_t *deck);
char *hit(deck_t *deck, hand_t *hand);
void addCardtoHand(deck_t *deck, hand_t *hand, char *card);
void printHand(hand_t *hand);
int getNumAces(hand_t *hand);
int getCount(hand_t *hand);
void setAceLow(hand_t *hand);
void setAceHigh(hand_t *hand);
void deleteHand(hand_t *hand);
```

### Implementation

We implement the cards module as two structs.
The *deck* itself is represented as a `struct deck` containing a counter and two hashtables.
A *hand* itself is represented as a `struct hand` containing a bag, two ints, and a char*.

To create a deck, the `initializeGame` function is called which returns the deck. To create the hands there is a 
`initializePlayer` and `initializeDealer` method which initializes their structs.

There are two methods to add a card to a hand. The first is the `hit` method which we use for our own dealer. It will return the name of the card it got from the deck. The other is the `addCardtoHand` method which is used when we are sent the name of the card and need to add it to our hand.

To extract information from the player or dealers hands there are two methods: `getCount` and `getNumAces` which will respectively tell you the count and number of aces in the player or dealers hand. If the player or dealer wants to set their ace permanently high or low, they can call the `setAceHigh` or `setAceLow` methods, respectively.

The `printHand` method prints a little syntax around the bag of cards in the players hand, and between cards, but mostly calls the `bag_print` function on each item by scanning the linked list.

The `deleteDeck` and `deleteHand` methods call the `hashtable_delete`, `bag_delete`, and `counters_delete` function on the necessary items in their struct. They conclude by freeing the `struct deck` and `struct hand`.

### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - compilation procedure
* `cards.h` - the interface
* `cards.c` - the implementation
* `cardstest.c` - unit test driver

### Compilation

To compile, simply `make cards.o`.

### Testing

The `cardstest.c` program tests each function that that the cards module exports. It tries sending in
bad information as well as good. Additional testing is also done as the player and dealer use specific
cards functions.

To test, simply `make`.

To test with valgrind, `make valgrind`.

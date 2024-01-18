# CS50 Blackjack 

### Dealer

The `dealer` is responsible for running the game of 21 from the server side. It allows players to connect, deals cards, sends messages to the player, receives messages from the player, and calculates the result of the game based on player and dealer point totals. It also considers edge cases, especially those involving Ace cards that can take on different values based on the value of other cards in the hand. The Dealer uses the deck and hand data structures defined in the cards module.
 
### Usage

The dealer module, implemented in `dealer.c`, implements a blackjack dealer. Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `dealer.c` and is not repeated here.

```c
static void parseArgs(const int argc, char* argv[], int* numGames, int* portNumber);
void dealerEvaluate(deck_t *deck, hand_t *dealerHand);
void playerEvaluate(deck_t *deck, hand_t *playerHand);
void handleAce(hand_t *hand, int *aceAddedHigh);
void calculateOutcome(hand_t *dealerHand, hand_t *playerHand, int new_socket);
void play(int new_socket, int numGames);
```

### Implementation

The `dealer` is responsible for running the game of 21 from the server side. It allows players to connect, deals cards, sends messages to the player, receives messages from the player, and calculates the result of the game based on player and dealer point totals. It also considers edge cases, especially those involving Ace cards that can take on different values based on the value of other cards in the hand. 
 
The Dealer is implemented in `dealer.c` and will use data structures from two helper modules: cards and network. The functions in dealer.c are as follows:
 
#### main: parses arguments and initializes other modules
Pseudocode: 
1. Initialize local variables 
2. Call functions to parse arguments, start/shutdown the server, and run the game
 
#### parseArgs: parses arguments from command line 
Pseudocode:
1. Check argument count 
2. For each argument, check valid type & range 
 
#### dealerEvaluate: evaluates dealer’s hand, hits while hand value <= 16, & checks where dealing with Aces is necessary 
Pseudocode:
1. If no Aces in starting hand, hit until hand value > 16, handling Aces as they come
2. If at least one Ace in starting hand, call handleAce to handle it 
3. Hit until hand value > 16, handling Aces as they come
4. If a temp Ace has been added by handleAce, call dealerEvaluate again to assign it a permanent value (now that more cards have been added to hand) 
 
#### playerEvaluate: evaluates player’s hand and deals with Aces 
Pseudocode: 
1. Get number of Aces in the player’s hand 
2. If there are more than 2, set them low until 2 unset Aces are remaining
3. If there are 2 Aces & the player would not bust if 12 points (11+1) were added to their hand, set one Ace high and one Ace low
4. If there are 2 Aces & the player would bust if 12 points (11+1) were added to their hand, set both Aces low
5. If there is one Ace & the player would not bust if 11 points were added to their hand, set the Ace high
6. If there is one Ace & the player would bust if 11 points were added to their hand, set the Ace low
 
#### handleAce: helps dealerEvaluate deal with Aces 
Pseudocode: 
1. If dealer's total with Ace counting as 11 is over 16 but below 22, Ace is set high and dealer will STAND
2. If dealer's total with Ace counting as 11 is over 22, Ace is set low and temporary high ace is voided
3. If dealer's total with Ace counting as 11 is <= 16, a temporary high ace is added to calculate point totals in dealerEvaluate (in case the dealer busts later on & the ace needs to be set low)
 
#### calculateOutcome: calculate end result of the game & send message to player
Pseudocode: 
1. Send a WIN result if player points > dealer points, both under 21, or if dealer busts
2. Send a LOOSE result if player points < dealer points, both under 21, or if player busts
3. Send a PUSH result if dealer & player points are tied under 21
 
#### play: loop through game actions until the specified number of games is played 
Pseudocode:
1. While the game is not finished:
2. Receive a JOIN message from the player (if not received, then send QUIT message)
3. Send a BEGIN message to the player 
4. Initialize game, player, and dealer 
5. Deal 2 cards to the player & send messages to communicate 
6. Deal one card face down & one card face up to dealer (only communicate face up card) 
7. Send DECISION message to player to prompt a decision 
8. Receive HIT/STAND decisions from the client 
9. While the player has not yet chosen to STAND:
9. If player chooses HIT, deal them another card & send a message 
10. If the player enters something other than HIT or STAND, prompt them to try again
11. After the decision has been made, call functions to evaluate both hands 
12. Print final scores of both players 
13. Call function to calculate the outcome of the game
14. Reset to play again by deleting both hands and the deck
15. After the specified amount of games is played, send QUIT message to player
16. Exit the play loop 
 
### Assumptions

Dealer.c and dealertest.c (or a player) must be run from different command line windows or devices.

### Files

* `Makefile` - compilation procedure
* `dealer.c` - the implementation
* `dealertest.c` - unit test driver
* `dealer-given` - Professor Pierson’s given dealer

### Compilation

To compile, simply `make all`.
The Makefile will compile both dealer and dealertest.

### Testing

The `dealertest.c` is a simplified player/client that communicates with the `dealer.c` via network. It connects to the dealer server by sending a JOIN message, receives messages from the dealer about the two hands, and makes a hardcoded decision to HIT or STAND. It stops playing when it receives a QUIT message from the dealer.
 
To test, run `./dealer numGames portNumber` first. The dealer will open the server for connection. 
Then, run `./dealertest` from another command line window to have the client join the game.
 
To test with valgrind, run `myvalgrind ./dealer numGames portNumber` & repeat steps above.
 
Testing output example below.
 
#### From dealer side:
f003y09@plank:~/cs50/labs/project-team-6/dealer$ ./dealer
Invalid number of arguments
Usage: ./dealer numGames portNumber
f003y09@plank:~/cs50/labs/project-team-6/dealer$ ./dealer 1 8086
port number: 8086
Setting up socket
Waiting for connection... Client connected. Server started.
Read: JOIN team_six
Sending message BEGIN
Sending message CARD Four of Hearts
Sending message CARD Jack of Clubs
Sending message DEALER Queen of Clubs
Sending message DECISION
Read: STAND
Player's Score: 14
Dealer's Score: 24
Sending message RESULT WIN
Sending message QUIT
Closing connected socket 
Closing listening socket 
 
#### From dealertest side:
f003y09@plank:~/cs50/labs/project-team-6/dealer$ ./dealertest
Got connection: 1
Sending message JOIN team_six
Read: BEGIN
Read: CARD Four of Hearts
Read: CARD Jack of Clubs
Read: DEALER Queen of Clubs
Read: DECISION
Sending message STAND
Read: RESULT WIN
Read: QUIT
Closing client
 

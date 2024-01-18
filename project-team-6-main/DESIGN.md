# CS50 BlackJack
## Design Specification
 
A reinforcement learning algorithm that learns to play a game optimally through trial and error. We will create a program that learns to play the popular card game 21 (also known as blackjack) in an optimal manner. Our program will play many games to learn which actions work well(e.g., the program wins), and which do not(e.g., the program loses).

### User interface
#### Player
The player’s only interface with the user is on the command-line; it must always have three arguments.
./player teamName IP teamNumber
For example, if the user wanted to play one game on port 8086:
Can use the following line to get the IP address
```
wget -O - -q https://checkip.amazonaws.com
```
$ ./player team6 IP 6
#### Dealer
 
The dealer’s only interface with the user is on the command-line; it must always have two arguments.
./dealer numGames portNumber
For example, if the user wanted to play one game on port 8086:
$ ./dealer 1 8086
 
### Inputs and outputs
#### Player
Input: When the player is both training and playing the player will read in the existing Q table and Q count from the file “qInfo” to either update the table or use the table for optimal playing, respectively. Besides this, the player takes no inputs aside from the command line arguments described above.
Output: When in training mode the player outputs its Q table and Q count when it has finished training to a file called “qInfo.” Additionally the player sends messages to the dealer and prints some messages to stdout.
#### Dealer
Input: The dealer takes no inputs aside from the command line arguments described above.
Output: The dealer sends messages to the player and prints some messages to stdout.
 
### Functional decomposition into modules
#### Player
We anticipate the following modules or functions:
1. `decision`, which stores the scores of the player and the dealer, the number of aces in the players hand, and the action they chose to make at each decision point in the game;
2. `main`, which initializes other modules to parse arguments, reads in the data for the Q table, runs the game, and writes out any updated data (if in training mode);
3. `parseArgs`, which parses command line arguments;
4. `readQ`, which reads in data from a file to update the Q table and Q count;
5. `writeQ`, which, if training, overwrites the Q data file with the information from the Q table and Q count;
6. `tokenizeInfo`, which loops through a given string and stores each word to a respective index in a given array of strings;
7. `runGame`, which connects through the server to the dealer, receives messages, and calls functions to make decisions/train/proceed through the game;
8. `handleDecision`, which, based on the player’s score and the dealer’s score (and the training mode condition) determines whether to hit or stand
9. `handleResult`, which, based on the outcome of the round, prints out the result and (if training) updates the Q-table and Q_count;
10. `updateQ`, which when called by `handleResult` updates the Q table with the data from decisions stored over the course of the round;
…as well as various small helper functions that assist in deleting the decisions and selecting random numbers for handleDecision in training mode.
 
As with `dealer`, it calls on helper modules *cards.c*, *network.c*, and *bag.c* for support in, respectively:
*cards.c*: keeping track of the deck of cards, the player’s hand, and the dealer’s hand, as well as the count of aces in the player’s hand
*network.c*: connecting to and communicating with the server/dealer
*bag.c*: storing the decisions to update the Q table and Q count
 
 
#### Dealer
We anticipate the following modules or functions: 
1. `main`, which initializes other modules to parse arguments, start/shutdown the server, and run the game;
2. `parseArgs`, which parses command line arguments;
3. `dealerEvaluate`, which evaluates the dealer's hand and chooses when to hit;
4. `playerEvaluate`, which evaluates Aces in the player's hand;
5. `handleAce`, which helps dealerEvaluate set values of Aces either high or low;
6. `calculateOutcome`, which calculates the final result of the game;
7. `play`, which performs all game actions in a loop that continues until the specified amount of games is played.
 
And some helper modules that provide data structures and support:
 
1. *cards*, a module providing the data structure to represent the in-memory deck of cards and players’ hands. Additionally, there are functions to access the items stored in a player’s hand (such as their cards).
2. *network*, a module providing the client and server with setup and shutdown functions as well as the ability to pass messages between each other through a socket
 
 
 
### Pseudo code for logic/algorithmic flow
#### Player
The player runs as follows: 
1. Initialize local variables, including a training boolean
2. Call parseArgs to parse command line arguments 
3. Call readQ to read in data from a file to be stored in the Q table
4. Call runGame to conduct the game itself. (See below)
5. If training, call writeQ to update the Q table file with updated data from new decisions made.
6. Free data allocated in parseArgs
 
…where parseArgs:
1. Checks argument count
2. For name and IP address, allocates memory and stores them
3. For port, updates it with +8080 to be a valid port and stores it
 
…where readQ:
1. Opens a file of the given name
2. Loops through every line in a file and tokenizes it (with tokenizeInfo)
3. Stores the data at each index of the tokenized array to corresponding variables
4. Updates Q-count and the Q table with the given data
 
…where writeQ:
1. Loops through every possible count of player points, of dealer points, of action points, and of ace counts
2. Indexes into the Q-count and Q table arrays with those indexes and stores the results
3. Writes out a line to the info file containing all of the above info, ordered as it should be read in by readQ
 
…where runGame: 
1. Initializes all necessary local variables
2. Opens a connection to the dealer
3. Sends a message to join with its name
4. Loops through the following until it receives QUIT:
	- BEGIN (clear out old decks, hands and bags: initializes new ones: updates the round count)
	- DECISION (send the existing hands and scores to handleDecision: hits if it returns 0, stands otherwise)
	- CARD (interprets the given card as it affects score using functions from `cards.c`: updates the player hand)
	- DEALER (interprets the given card as it affects score using functions from cards.c: updates the dealer hand)
	- RESULT (sends the outcome and the bag of decisions to handleResult)
5. Once it receives QUIT or an unknown command, call endGame.
 
…where tokenizeInfo:
1. Parses the given string word-by-word
2. Saves each word to an index in a character array that exists outside of the function
 
…where handleDecision:
1. Returns 0 if the player’s score is less than 12
2. Returns 1 if the player’s score is equal to 12, equal to 21, or greater than 21 with no aces counted as high
3. Sets an ace to lower and runs handleDecision again with the resulting updated scores if the number of aces in the player hand is greater than 0 and their score is greater than 21
4. If training, 
- chooses a random action between 0 and 1
- creates a decision reflecting that
- adds the decision to the bag
5. Otherwise, check the Q table for the values indexed at the current scores and ace count for BOTH actions, and returns whichever one stores a higher result value
 
…where handleResult:
1. Converts the string for the outcome to a reward count:
- 1 if win
- 0 if pushed
- -1 if lost/busted
2. If training, sends that reward to updateQ
 
…where updateQ:
1. Extracts every decision from the corresponding bag
2. Updates the Q table and Q-count with the mathematical formulas described in the Implementation Spec, using the values stored in each decision
3. Frees the memory for each decision
 
…where endGame:
1. Frees the bag, both hands, and the deck
2. Closes out the client server
 
#### Dealer
 
The dealer will run as follows:
1. Initialize local variables
2. Call parseArgs to parse command line arguments 
3. Call start_server to open the server for player connection 
4. Call play to run game actions in a loop
5. Shut down the server
 
where parseArgs:
1. Checks argument count 
2. For each argument, check valid type & range 
 
where dealerEvaluate: 
1. Hits until hand value > 16, handling Aces as they come
2. Calls handleAce when needed to set Aces high, temporarily high, or low
3. If a temp Ace has been added by handleAce, call dealerEvaluate again to assign it a permanent value (after more cards have been added to hand) 
 
where playerEvaluate:
1. Gets number of Aces in the player’s hand 
2. If there are more than 2, set them low until 2 unset Aces are remaining
3. If there are 2 Aces & the player would not bust if 12 points (11+1) were added to their hand, sets one Ace high and one Ace low
4. If there are 2 Aces & the player would bust if 12 points (11+1) were added to their hand, sets both Aces low
5. If there is one Ace & the player would not bust if 11 points were added to their hand, sets the Ace high
6. If there is one Ace & the player would bust if 11 points were added to their hand, sets the Ace low
 
where handleAce:
1. Helps dealerEvaluate deal with Aces 
1. If dealer's total with Ace counting as 11 is over 16 but below 22, Ace is set high and dealer will STAND
2. If dealer's total with Ace counting as 11 is over 22, Ace is set low and temporary high ace is voided
3. If dealer's total with Ace counting as 11 is <= 16, a temporary high ace is added to calculate point totals in dealerEvaluate (in case the dealer busts later on & the ace needs to be set low)
 
where calculateOutcome:
1. Sends a WIN result if player points > dealer points, both under 21, or if dealer busts
2. Sends a LOOSE result if player points < dealer points, both under 21, or if player busts
3. Sends a PUSH result if dealer & player points are tied under 21
 
where play:
1. Loops through game actions until the specified number of games is played 
2. Receives a JOIN message from the player (if not received, then send QUIT message)
3. Sends a BEGIN message to the player 
4. Initializes game, player, and dealer 
5. Deals cards to player and dealer
7. Sends DECISION message and receives HIT/STAND decisions
8. Evaluates both hands and calculates the outcome of the game
9. Resets to play again by deleting both hands and the deck
10. After the specified amount of games is played, sends QUIT message to player
11. Exits the play loop 
 
### Major data structures
#### Cards
 
The key data structs are the *deck*, which keeps track of the cards and the *hand* keeps track of the information of a player’s or dealer’s hand.
The deck is a… 
- *counters* for card numbers and the number of times that card has been seen
- *hashtable* of card numbers to the name of their card
- *hashtable* of name of a card to their card numbers
The hand is a…
- *bag* for the cards that a player or dealer holds
- *char* * for the name of the hand (“Player” or “Dealer”)
- *int* for the number of aces in the hand
- *int* for the count of the hand (not including aces)
 
#### Player
The sole ‘new’ data struct for `player` is the *decision* structure, which keeps track of the player and dealer hands surrounding a decision in order to later update the Q table with decisions made. More specifically, decisions store:
- * the player’s score at that point in the game
- * the visible dealer’s score at that point in the game
- * the action taken at that point in the game (as an integer: 0 for hit, 1 for stand)
- * the number of high aces (valued at 11 points) in the player’s hand
When in training mode, these decisions are stored in a *bag* and called at the end of a round using the `updateQ` function.
 
#### Network
 
In the Unix socket API, the IPv4 socket address structure is named sockaddr_in and is defined by including the netinet/in.h header file. The standard definition is the following:
 
struct in_addr {
  in_addr_t s_addr;  /* 32-bit IPv4 network byte ordered address */
};
 
struct sockaddr_in {
  uint8_t        sin_len;     /* length of structure (16) */
  sa_family_t    sin_family;  /* AF_INET */
  in_port_t      sin_port;    /* 16 bit TCP or UDP port number */
  struct in_addr sin_addr;    /* 32 bit IPv4 address */
  char           sin_zero[8]; /* not used, but always set to zero */
};
 
When setting up our client server connection, we fill in a majority of this structure to create our sockets. 
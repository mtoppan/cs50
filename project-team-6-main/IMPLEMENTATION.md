# CS50 BlackJack
## Implementation Specification
In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.
Implementation may include many topics; not all are relevant to the BlackJack player.
Here we focus on the core subset:
 
-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan
 
### Data structures
#### Cards
We create two data structures: a `deck` and a `hand`. The `deck` allows us to deal cards and keep track of them, and a `hand` that tells us information about what the player or dealer holds. These data structures are composed of other data structures such as `hashtables`, `counters`, and `bags`.
 
#### Player
We create one data structure, a `decision` struct. This allows us to track a player’s decisions over the course of the game by recording their score, the dealer’s score, the number of aces (set to high) that they have in hand, and the decision that they make (to hit or to stand) at that point. By storing these in a `bag`, we can retrieve them at the end of each round and log them alongside the round outcome to update the Q-table. 
While not a structure, we also make significant use of global arrays to quantify and qualify the above decisions. These arrays— the `Q-count` and `Q` table are updated during training and stored such that the user may index into them to determine whether hitting or standing with a given game set-up will be more likely to lead to a positive outcome.
### Control flow
#### Player
We create a driver `player` through which the BlackJack game runs. This script can play in a “training” or non-training mode, which is determined by a boolean and dictates how the computer responds to cards being dealt. The `player` receives messages from the dealer, determines how to respond, and proceeds to do so until the set number of games have been played and the dealer terminates the game. 
In order to make the most informed decisions, the `player` makes use of a `Q` table— described above and set as a global variable. This array takes in the decisions made by the player in training mode to catalog the best action to take to receive the highest likelihood of winning. To keep track of previous decisions, the table is read in at the beginning of every round, and is overwritten with a more progressed version at the end of every training round. 
 
The Player is implemented in `player.c` and will use data structures from `cards.c`, `network.c`, and `libcs50`. The functions in `player.c` are as follows:
 
##### `main`: 
This takes in all of the necessary command-line arguments and runs through the necessary primary functions to set up and make use of the given data. If any errors are encountered in parsing arguments or in running the game, it returns non-zero: otherwise, it returns zero. 
 
Parse arguments and allocated memory for the player name, the IP address, and the port, as needed
Read in the necessary data for the Q table
Run the game
If training, write out the updated data to the data file for the Q table
Free any memory from the initial variables
 
##### `readQ`
Pseudocode:
* Open the file with the provided name
* If the file is successfully reached:
	* Initialize a string and an array of strings
	* Read through every line in the given file until NULL is reached
		* Allocate space for the array
		* Tokenize the line and save its info into the array
		* Save every element of the array to a respective variable.
		* NOTE: the information read in should be in precisely the same format as it is written to the file in writeQ, below. Refer to the variable types used there to ensure that the data is read in correctly.
		* Update the newly initialized Q-count and Q table with the given values, setting every player score, dealer score, action type, and ace count to the correct occurrence count and reward. 
		* Free the line and the array
	* Close the file
 
##### `writeQ`
Pseudocode:
* Open the file with the provided name
* If the file is successfully reached:
	* Loop through all possible player scores, all possible dealer scores, a count of all possible action types, and all possible ace counts
		* Save the count indexed at the culmination of the above values to a variable
		* Save the reward indexed at the culmination of the above values to a variable
		* Print the player score, dealer score, action, ace count, occurrence count, and reward average as a line to the given file
	* Close the file
 
##### `parseArgs`
The purpose of this function is to validate the inputs sent in via the command line.
Ensure that the right number of functions was received
Allocate space for the name and IP address
Copy the name and IP address into the correct variables, created in `main`
Update the port number by adding 8080 to it, and save it to the correct variable in `main`
Return zero if successful and non-zero otherwise.
 
##### `runGame`
The entirety of the game is run through this function: the network is set up, messages are received and sent, and all decisions are made. Thus, it makes significant use of `network.c` and `cards.c`. 
 
Pseudocode:
	Initialize the socket and client information
	Initialize action variables, as well as a boolean to check for connection
	Set up a series of strings, to look for after connecting to the dealer
Connect to the server
Send the message to join with the players name
Initialize a deck of cards
Initialize a hand of cards for the player and a hand for the dealer
Initialize a bag in which to store the decisions that are made
Set up a round counter
Initialize a buffer into which to read the messages from the dealer
Receive the first message from the dealer
While the message received is not “QUIT”:
	Tokenize the information received into an array of character arrays
	Check the first word from each line
	If it’s “BEGIN”
		Delete all old data structures (deck, hands, bag)
		Initialize new data structures
		Increment the round count
		Print that the round has begun
	If it’s “DECISION”
		Get the player’s score and the dealer’s score
		Send them to handleDecision, along with the bag of decisions, the training boolean, and the player’s hand
		If the result of handleDecision is 0, send “HIT” to the dealer
		Otherwise, send “STAND”
	If it’s “CARD”
		Reconstruct the tokenized card information into a new string
		Add the card to the player’s hand
	If it’s “DEALER”
		Reconstruct the tokenized card information into a new string
		Add the card to the dealer’s hand
	If it’s “RESULT”
		Send the outcome, the training boolean, and the bag of decisions to handleResult
	Otherwise, end the game and return and error
Once quit is received, end the game.
Return 0.
 
##### Helper functions
The functions detailed below serve as essential helpers for the main game.
 
###### `tokenizeInfo`
The `tokenizeInfo` function takes in the string of information received from the dealer and parses it such that the player knows how to respond. This function is derived from the function of the same purpose used in the Querier assignment (Lab 6).
 
Pseudocode:
* take in a string and an array of strings
* Setup integer variables for word count and string length, and a boolean to track whether the index is in a word or not. 
* Loop through the length of the string.
	* if the character at that point is not a space, 
* add the word at that pointer to index = wordCount in the array of strings
* increment word number
		* set inWord to true
	* otherwise, if it is in a word and pointing to a space,
		* change the character in the input string to a null terminator (\0)
		* set inWord to false
 
###### `handleDecision`
The `handleDecision` function takes the player’s score in their hand and the dealer’s visible score and decides whether to hit or stand based on the following logic/pseudocode:
 
* If the player’s score is less than 12, HIT. Return 0.
* If the player’s score is exactly 12, exactly 21, or greater than 21 with no aces left to lower, STAND. Return 1. 
* If the player’s score is more than 21 and they have non-low aces in their hand, invoke setAceLow on the player’s hand and run handleDecision on the updated hand/scores.
* If the player is in training mode, randomly select between 0 (HIT) and 1 (STAND). Return the result.
* If none of the above conditions are met, index into the Q table using the player’s score, the dealer’s score, and the number of high aces in the player’s hand (using getNumAces from `cards.c`) for each of the possible actions.
	* If the outcome score is higher for hitting than standing, return 0. 
	* Otherwise, return 1. 
 
###### `handleResult`
The `handleResult` function takes in the outcome of the game as reported by the dealer, the bag of decisions, and the boolean to recognize if training mode is active or not. It prints out a string corresponding the outcome and, if in training mode, calls updateQ.
 
Pseudocode: 
If the outcome is a win, the reward is 1. Print the result.
If the outcome is a lose or a bust, the reward is -1. Print the result.
If the outcome is a push, the reward is 0. Print the result.
If the player is set to be training, send updateQ the bag of decisions and the integer version of the outcome. 
 
###### `endGame`
The `endGame` function is a means of terminating the game by freeing any memory allocated during the game. It is only called when an error, an unfamiliar command, or QUIT is encountered. 
 
Free the bag of decisions
Free the player’s hand
Free the dealer’s hand
Free the deck
Close the client connection
 
##### Helper helpers
The functions detailed below are invoked within helper functions in order to increase overall efficiency of the calls and to allow for greater clarity.
 
###### `randomPick`
The `randomPick` function is a helper for the `training` version of `handleDecision`.
It randomly selects between 1 and 0.
It returns the integer, to be read by `handleDecision` and interpreted as ‘stand’ or ‘hit’, respectively.
 
###### `updateQ`
The `updateQ` function is drawn from the given information for the Game of 21 assignment, but differs in part as, rather than taking the recommended inputs, it takes the bag of decisions and reads through the data within to access scores, actions, and ace records.
 
Pseudocode:
* Takes in the bag of decisions and the reward as interpreted to an integer
* While the bag is not empty, extract a decision from it
	* Take the data stored in that decision structure and update Q_count and the Q table.
 
To update Q_count and the Q table, use the following mathematical functions, as provided in the assignment README.
 
```c
Q_count[player_points][dealer_points][action] += 1; //increment count
Q[player_points][dealer_points][action] += (1/(float)Q_count[player_points][dealer_points][action]) * (reward - Q[player_points][dealer_points][action]) //update reward
}
```
 
###### `decisionsDelete`
The `decisionDelete` function is a helper for the bag_delete function. It takes in an object, meant to be a decision struct, and frees the memory allocated to it.
 
#### Dealer
The `dealer` is responsible for running the game of 21 from the server side. It allows players to connect, deals cards, sends messages to the player, receives messages from the player, and calculates the result of the game based on player and dealer point totals. It also considers edge cases, especially those involving Ace cards that can take on different values based on the value of other cards in the hand. 
 
The Dealer is implemented in `dealer.c` and will use data structures from two helper modules: cards and network. The functions in dealer.c are as follows:
 
### main: parses arguments and initializes other modules
Pseudocode: 
1. Initialize local variables 
2. Call functions to parse arguments, start/shutdown the server, and run the game
 
### parseArgs: parses arguments from command line 
Pseudocode:
1. Check argument count 
2. For each argument, check valid type & range 
 
### dealerEvaluate: evaluates dealer’s hand, hits while hand value <= 16, & checks where dealing with Aces is necessary 
Pseudocode:
1. If no Aces in starting hand, hit until hand value > 16, handling Aces as they come
2. If at least one Ace in starting hand, call handleAce to handle it 
3. Hit until hand value > 16, handling Aces as they come
4. If a temp Ace has been added by handleAce, call dealerEvaluate again to assign it a permanent value (now that more cards have been added to hand) 
 
###playerEvaluate: evaluates player’s hand and deals with Aces 
Pseudocode: 
1. Get number of Aces in the player’s hand 
2. If there are more than 2, set them low until 2 unset Aces are remaining
3. If there are 2 Aces & the player would not bust if 12 points (11+1) were added to their hand, set one Ace high and one Ace low
4. If there are 2 Aces & the player would bust if 12 points (11+1) were added to their hand, set both Aces low
5. If there is one Ace & the player would not bust if 11 points were added to their hand, set the Ace high
6. If there is one Ace & the player would bust if 11 points were added to their hand, set the Ace low
 
###handleAce: helps dealerEvaluate deal with Aces 
Pseudocode: 
1. If dealer's total with Ace counting as 11 is over 16 but below 22, Ace is set high and dealer will STAND
2. If dealer's total with Ace counting as 11 is over 22, Ace is set low and temporary high ace is voided
3. If dealer's total with Ace counting as 11 is <= 16, a temporary high ace is added to calculate point totals in dealerEvaluate (in case the dealer busts later on & the ace needs to be set low)
 
###calculateOutcome: calculate end result of the game & send message to player
Pseudocode: 
1. Send a WIN result if player points > dealer points, both under 21, or if dealer busts
2. Send a LOOSE result if player points < dealer points, both under 21, or if player busts
3. Send a PUSH result if dealer & player points are tied under 21
 
###play: loop through game actions until the specified number of games is played 
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
 
 
### Other modules
<a id="cards"></a>
### cards
 
We create a re-usable module `cards.c` to keep track of the deck as well as the player and dealer’s hands. We chose to write this as a separate module, in `../cards`, to encapsulate all the knowledge about how to handle the deck and cards.
 
Pseudocode for `initializeGame`:
 
   Allocate memory for the deck struct
   Call `createCards`: initializes the counter by creating a counter for each cardNumber(0-51) and settings its count to 1 (means unseen)
   Call `createLUT`: initializes both look up tables. One maps the card number to the name of the cards and the other maps the name of the card to the card number
   Return the deck
 
Pseudocode for `intializeDealer`:
 
   Check that the deck is not null
Allocate memory for a hand struct
Initialize the bag
Set the count and number of aces to zero
Set the name of the hand to “Dealer”
Return the hand
 
 
Pseudocode for `initializePlayer`:
 
   Check that the deck is not null
Allocate memory for a hand struct
Initialize the bag
Set the count and number of aces to zero
Set the name of the hand to “Player”
Return the hand
 
Pseudocode for `hit`:
 
   Call `getCard`: randomly chooses a number from 0-51 and if the card has not been dealt returns the associated card. Keeps randomly choosing a number until finds a cards that has not been dealt. Adds the card to the hand it was called on
   Return the name of the card
 
Pseudocode for `addCardtoHand`:
 
   Check that the card name sent in is a valid card
	Uses the LUT to find the card’s ID number
	Marks the card as seen
	Updates the hand’s count
	Add the card to the hand’s bag
 
Pseudocode for `printHand`:
 
   Call bag print function on the hand’s bag of cards
 
Pseudocode for `getNumAces`:
 
   Return numAces in the hand
 
Pseudocode for `getCount`:
 
   Return count of the hand
 
Pseudocode for `setAceLow`:
 
   Check that the hand does contain at least one ace
	Decrement the number of aces
	Add a value of one to the count
 
Pseudocode for `setAceHigh`:
 
   Check that the hand does contain at least one ace
	Decrement the number of aces
	Add a value of eleven to the count
 
Pseudocode for `deleteDeck`:
 
   Free the counter
   Free LUT1
   Free LUT2
   Free the deck
 
Pseudocode for `deleteHand`:
 
   Free the bag
   Free the hand
 
<a id="network"></a>
### network
Pseudocode for `start_server`:
 
   Create socket file descriptor with socket()
   Attach socket to the PORT with setsockopt()
   Fill the sockaddr_in struct with sin_family, sin_addr, and sin_port
   Bind socket to the port with bind()
   Listen
   If a new client connects, create new socket with accept()
 
Pseudocode for `shutdown_server`:
 
   Close the new socket (listening socket) using close()
   Shutdown the listening socket with shutdown()
 
Pseudocode for `connect_to_server`:
 
   Create a new socket with socket()
   Fill in sockaddr_in struct with sin_family and sin_port
   Convert IPv4 and IPv6 addresses from text to binary form with inet_pton
   Create a new client file descriptor with connect()
 
Pseudocode for `close_client`:
 
   Close the client file directory with close()
 
Pseudocode for `send_message`:
 
   Send a passed in message through a given socket using send()
 
Pseudocode for `recieve_message`:
 
   Reset the passed in buffer using memset()
   Read the socket and put message into the buffer using read
   Check that message was recieved
 
Pseudocode for `send_join_message`:
 
   Malloc memory for the size of a passed in team name and the word JOIN
   Create a string in the format “JOIN (team name)” 
   Call send_message
   Free the join message
 
 
#### libcs50
We leverage the modules of libcs50, most notably `counter`, `bag` and `hashtable`.
See that directory for module interfaces.
 
### Function prototypes
#### Player
Detailed descriptions of each function’s interface is provided as a paragraph comment prior to each function’s implementation in player.c and is not repeated here. 
 
```c
void readQ(char* fileName);
void writeQ(char* fileName);
int parseArgs(int argc, char* argv[], char** NAME, char** IP, int* PORT);
int runGame(char* NAME, char* IP, int PORT, bool training);
int tokenizeInfo(char* input, char** info_array);
int randomPick();
void updateQ(bag_t* decisions, int reward);
int handleDecision(bool training, int total_score, int dealer_score, bag_t* decisions_made, hand_t* hand);
void handleResult(char* result, bool training, bag_t* decisions);
void endGame(hand_t* player_hand, hand_t* dealer_hand, deck_t* deck, bag_t* decisions_made, int client_fd);
void decisionsDelete(void* decision);
```
 
#### Dealer
 
Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `dealer.c` and is not repeated here.
```c
static void parseArgs(const int argc, char* argv[], int* numGames, int* portNumber);
void dealerEvaluate(deck_t *deck, hand_t *dealerHand);
void playerEvaluate(deck_t *deck, hand_t *playerHand);
void handleAce(hand_t *hand, int *aceAddedHigh);
void calculateOutcome(hand_t *dealerHand, hand_t *playerHand, int new_socket);
void play(int new_socket, int numGames);
```
 
#### Cards
Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `cards.h` and is not repeated here.
 
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
 
#### Network
Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `network.h` and is not repeated here.
 
```c
void start_server(const int port, int* server_fd, int* new_socket);
void shutdown_server(int new_socket, int server_fd);
bool connect_to_server(const char *server_addr, const int port, int* client_fd, int* sock);
void close_client(int client_fd);
void send_message(char* message, int sock);
void recieve_message(char* buffer, int sock);
void send_join_message(char* team_name, int sock);
```
 
### Error handling and recovery
All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.
 
Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.
 
All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.
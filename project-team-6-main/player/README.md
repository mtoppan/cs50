# CS50 BlackJack

### Player

The `player` is a driver class through which the game of 21 is run.
It makes use of two `float arrays`, Q and Q_count, which store the average outcome (as an integer) for every flexible combination of player score, dealer score, ace cound, and action (hit or stand) and the number of times each combination occurs, respectively. 
In order to update the Q table and Q_count, it uses a `decision` struct, detailed below, that stores all index values needed for both arrays. A new `decision` is made for every update to the score counts, and is stored in a `bag`. 

### Usage

The *player* module, defined and implemented in `player.c`, is run on the command line with the following usage structure:

```bash
$ ./player PLAYER_NAME IP_ADDRESS PORT
```

It manages the game of 21 as a whole, implementing the `cards` and `network` modules in order to conduct the card interactions of the game and the networking elements. More specifically, it runs the process of *training* the player to make informed decisions relative to the dealer; it receives information from the dealer and established the best-fit responses; and it initiates and concludes the entirety of the game on the side of the user. In order to train, it also creates a `decision` struct that holds the player's cummulative score, the dealer's known score, the action made, and the number of aces in hand at each step of the game. These are made and stored in a `bag` during training in order to reference back to them later and update the training data. 
In order to continuously build on the training data, this information is written out to a training data file, `qInfo`, after each round and read into two `float` arrays— henceforth referred to as the Q-count and Q table, which look at occurrences and resulting rewards of different decisions— at the initiation of a player. 

It exports the following functions:

```c
void readQ(char* fileName);
void writeQ(char* fileName);
int parseArgs(int argc, char* argv[], char** NAME, char** IP, int* PORT);
int runGame(char* NAME, char* IP, int PORT, bool training);
int tokenizeInfo(char* input, char** info_array);
int handleDecision(bool training, int total_score, int dealer_score, bag_t* decisions_made, hand_t* hand);
void handleResult(char* result, bool training, bag_t* decisions);
void endGame(hand_t* player_hand, hand_t* dealer_hand, deck_t* deck, bag_t* decisions_made, int client_fd);
int randomPick();
void updateQ(bag_t* decisions, int reward);
void decisionsDelete(void* decision);
```

### Implementation

We implement the bulk of the `player` functionality in `runGame`, which is called by `main` and which takes the information received from the commandline and parsed by `parseArgs`, as well as a boolean for training that guides the player's interactions with the Q-table and decision making process. 

`runGame` connects to the server and dealer using functions from `network.c`, and loops through messages received to respond in the corresponding manner.

If it receives "BEGIN", it frees then reinitializes the deck, player hand, dealer hand, and bag of decisions. It then increments and prints the round number.

If it receives "DECISION", it calls `handleDecision` which, depending on the player's score, the dealer's score, and the training condition, responds with an integer representing a *hit* or a *stand*. If the training condition is active, it chooses randomly and stores the decision in the bag; if it is not, it indexes into the Q table to find the action with the best outcome for the current game state.

If it receives "CARD", it updates the player's card count by calling on functions from `cards.c`.

If it receives "DEALER", it updates the dealer's card count by calling on functions from `cards.c`.

If it receives "RESULT", it calls `handleResult` to convert the outcome into an integer. If the training condition is active, it invokes `updateQ` to add the decisions stored in the bag to the Q table and Q count. 

If it receives an unknown command or "QUIT", it calls `endGame` which frees all structs and closes out the connection to the server.

### Assumptions

No assumptions beyond those that are clear from the spec. Any errors or exceptions are anticipated within other modules— e.g., issues encountered with connecting to the server are managed by `network`. 
If anything, we assume that any dealer reached is functioning as desired.

### Files

* `Makefile` - compilation procedure
* `player.c` - the implementation and interface
* `qInfo` - the training data file, storing all information needed for the Q-table

### Training

To train the program, we set the `training` variable in main to *true*. This instructs the player to track decisions, randomly choose between hit and stand when prompted, and write out update results to the Q-table data file. In order to play based on that knowledge rather than at random, we seet `training` to *false*.
We decided to run 5304 test games to train the player on the basis of the following logic:
Given that there are 52 cards in the deck, there are 52*51 possible permutations for the first two cards dealt. Thus, all possible combinations of cards comes to 2652 options. Additionally, given those first two cards, the dealer must choose between two options— stand (1) or hit (0) at least once. To cover both eventualities, we multiply the number of permutations by two. This gives us a total 5304 test games to run against the dealer. Lastly, since each time we run in training mode we first read in the Q table, we are able to improve our model each time we train.

### Compilation

To compile, simply `make` within the player folder.

### Testing

As stated in the Implementation Spec, since the player functions largely in tandem with the dealer, the individual functions may be tested in checks within a non-networked setting by disabling the `runGame` function and individually testing subsections of the code. However, to test it as a whole it is best seen in practice alongside the dealer. 

To test, setup the dealer in a separate terminal with the following command:

```bash
./dealer 1 1 8086
```
...then run the player in the original terminal with the following command:

```bash
./player team_6 129.170.64.104 6
```

To test with valgrind, follow the above but accompany the command with `myvalgrind` as follows:

```bash
myvalgrind ./player team_6 129.170.64.104 6
```

52*51 is the number of possible hands you're dealt, and *2 for hit versus stand, should cover absolutely all possible permutations of starting cases and ensuing decisions.
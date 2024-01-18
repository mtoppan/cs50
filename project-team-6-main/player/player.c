/* 
 * player.c - player for the BlackJack game
 *
 * The `player` runs the Blackjack game from the client side. It connects with the dealer
 * via the server, initializes deck and hand structures, calculates totals based on cards
 * dealt and makes decisions using data generated through training cycles. It considers such
 * cases as the adjusting Ace and accounts for all possible commands received. In training
 * mode, it documents decisions using a decisions structure and a bag and updates them
 * following each game.
 *
 * Macy Toppan
 * CS 50, Fall 2022
 */

#include "../libcs50/bag.h"
#include "../libcs50/mem.h"
#include "../libcs50/file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <unistd.h>
#include "../cards/cards.h"
#include "../network/network.h"


//Global variables for Q and Q-count
static float Q[21][12][2][5] = {0}; 
static int Q_count[21][12][2][5] = {0}; 

//Decision struct, to keep track of choices made and eventually update the Q values
typedef struct decision{
    int action;
    int player_score;
    int dealer_score;
    int aces;
} decision_t;

//Function declaration: all functions below are explained in detail at their respective locations in the code
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

//*******************main*******************//
//Body of the program, taking inputs from the command line
//Inputs: player name NAME, ip address for dealer IP, port for dealer PORT
//Outputs: returns 0 if the game is run smoothly and non-zero otherwise
//
//Purpose: run the necessary functions to parse arguments, setup Q data, run the game, and update the Q data. 
int main (const int argc, char* argv[]){
    char* NAME;
    char* IP;
    int PORT;
    bool training = false;

    int args = parseArgs(argc, argv, &NAME, &IP, &PORT);
    if (args != 0){
        return args;
    }

    char* qInfo = "qInfo";
    int end;

    readQ(qInfo);
    end = runGame(NAME, IP, PORT, training);
    if(training){
        writeQ(qInfo);
    }

    mem_free(NAME);
    mem_free(IP);

    return end;
}

//*******************readQ*******************//
//Brings the necessary data into the Q-table and Q-count for reference in play
//Inputs: file name "fileName" to look to for Q-table and Q-count info
//Outputs: none— updates existing global variables Q and Q_count
//
//Purpose: read in data from a given file to fill in Q data for play, reading line-by-line for information printed in the following format:
//   player_score dealer_score action ace_count occurrence_count reward
void readQ(char* fileName){
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL){
        fclose(fp);
        fprintf(stderr, "No file exists from which to read the Q_table.\n");
    }
    else{
        char **qArray;
        char* line;
        while ((line = file_readLine(fp)) != NULL){
            qArray = malloc((strlen(line) + 1)/2 * sizeof(char*));
            tokenizeInfo(line, qArray);
            int p = atoi(qArray[0]);
            int d = atoi(qArray[1]);
            int act = atoi(qArray[2]);
            int ace = atoi(qArray[3]);
            int count = atof(qArray[4]);
            float reward = atof(qArray[5]);
            Q_count[p][d][act][ace] = count;
            Q[p][d][act][ace] = reward;
            
            free(line);
            free(qArray);
        }
        fclose(fp);
    }
}

//*******************writeQ*******************//
//Updates the Q-data file (given) with information from the Q-table and Q-count
//Inputs: file name "fileName" to which to print Q-table and Q-count info
//Outputs: none— overwrites the file directly
//
//Purpose: write out data to a given file to catalogue Q-data s.t. it may be saved and later read back in for play, writing line-by-line in the following format:
//    player_score dealer_score action ace_count occurrence_count reward
void writeQ(char* fileName){
    FILE* fp = fopen(fileName, "w");
    if (fp == NULL){
        fclose(fp);
        fprintf(stderr, "Cannot create or find a file to which to write.");
    }
    else{
        // for p in range 20
        for (int p = 0; p <=20; p++){
            //for d in range 32
            for (int d = 0; d <=11; d++){
                //for act in range 0-1
                for (int act = 0; act <=1; act++){
                    //for ace in range 4
                    for (int ace = 0; ace <=4; ace++){
                        int count = Q_count[p][d][act][ace];
                        float reward = Q[p][d][act][ace];
                        fprintf(fp, "%d %d %d %d %d %f\n", p, d, act, ace, count, reward);
                    }
                }
            }
        }
        fclose(fp);
    }
}

//*******************parseArgs*******************//
//Given the command line inputs and pointers to various variables, validate and save out the info for use in setting up the network
//Inputs: argc (commandline arg count), arv[] (command line argument array), NAME (to which to save the player name), IP (to which to save the IP address), and PORT 
//   (to which to save the modified PORT address)
//Outputs: an integer, returning 0 if all memory is successfully allocated and all variables successfully saved: non-zero if any errors are encountered
//
//Purpose: set up the data in a format that the network can use to set up the game
int parseArgs(int argc, char* argv[], char** NAME, char** IP, int* PORT){
    if (argc != 4){
        fprintf(stderr, "Wrong number of arguments. Usage: ./player name ip_address port_number\n");
        return -1;
    }
    // read in arguments: HOW DO WE VALIDATE THEM?
    *NAME = mem_malloc_assert(strlen(argv[1])+1, "failed to allocate memory for name\n");
    strcpy(*NAME, argv[1]);
    *IP = mem_malloc_assert(strlen(argv[2])+1, "failed to allocate memory for IP address\n");
    strcpy(*IP, argv[2]);

    //if port is supposed to have 8080 added into it 
    *PORT = atoi(argv[3]) + 8080;

    return 0;
}

//*******************runGame*******************//
//Given in the information saved from the command line, connect to the network and play through the game (making all necessary decisions) as many times as the dealer allows.
//Inputs: player name NAME, IP address IP, port address PORT, boolean to know whether to be in training mode or not
//Outputs: an integer, returning 0 if the game plays through successfully and non-zero if any unexpected commands are received from the dealer
//
//Purpose: Connect to the network and check for specific messages, responding appropriately to each in order to correctly play through the game (equipped for both a 
//   random decision training mode and a planned mode, deciding based on Q-table information);
int runGame(char* NAME, char* IP, int PORT, bool training){
// set up the server stuff
    int sock = 0;
    int client_fd;

    //alll of the below is for reference for message comparisons when the time comes
    char* quit_message = "QUIT";
    char* begin_message = "BEGIN";
    char* card_message = "CARD";
    char* dealer_message = "DEALER";
    char* decision_message = "DECISION";
    char* result_message = "RESULT";

    int act;
    bool connect = false;

    connect = connect_to_server(IP, PORT, &client_fd, &sock);
    printf("Connect status: %d", connect);

//  send "join" message with name to dealer
    printf("Got connection\n");
    printf("Waiting to join\n");
    printf("Sending player info\n");

    send_join_message(NAME, sock);
    
//  initialize a deck and hands
    deck_t* deck = initializeGame();

    hand_t* player_hand = initializePlayer(deck);
    hand_t* dealer_hand = initializeDealer(deck);

    bag_t* decisions_made = bag_new();
    
    int round = -1;

    char buffer[1024] = { 0 };
    recieve_message(buffer, sock);

// while dealer does not send "quit" message
    // while ( strcmp(read(sock, buffer, 1024), quit_message) != 0 ){
    while ( strcmp(buffer, quit_message) != 0 ){
//  parse it: get the first word to know what to do
        char *info[strlen(buffer)/2];
        tokenizeInfo(buffer, info);
        
// 	recieve "begin" message from dealer
        if (strcmp(info[0], begin_message)==0){
            //delete the decision bag
            bag_delete(decisions_made, decisionsDelete);
            //delete the deck
            deleteDeck(deck);
            //free the old p hand
            deleteHand(player_hand);
            //free the old d hand
            deleteHand(dealer_hand);
            //make a hand struct for the player
            player_hand = initializePlayer(deck);
            //make a hand struct for the dealer
            dealer_hand = initializeDealer(deck);
            //make a new deck (reshuffle)
            deck = initializeGame();
            //round number
            round += 1;

            //to keep track of decisions made and input them to Q table later
            decisions_made = bag_new();

            //announce the round
            fprintf(stdout, "\nBeginning Round %d\n", round);
        }
//  receive "decision" message from dealer
        else if (strcmp(info[0], decision_message)==0){
            int total_score = getCount(player_hand) + (11*(getNumAces(player_hand)));
            int dealer_score = getCount(dealer_hand) + (11*(getNumAces(dealer_hand)));
            act = handleDecision(training, total_score, dealer_score, decisions_made, player_hand);
            if (act == 0){
                send_message("HIT", sock);
            }
            else{
                send_message("STAND", sock);
            }
        }
//  receive "card" message from dealer
        else if (strcmp(info[0], card_message)==0){
            char card[18];
            sprintf(card, "%s %s %s", info[1], info[2], info[3]);
            addCardtoHand(deck, player_hand, card);
        }
//  received "dealer" message from dealer
        else if (strcmp(info[0], dealer_message)==0){
            char card[18];
            sprintf(card, "%s %s %s", info[1], info[2], info[3]);
            addCardtoHand(deck, dealer_hand, card);
        }
//  received "result" from dealer
        else if (strcmp(info[0], result_message)==0){
            handleResult(info[1], training, decisions_made);
        }

        else {
            fprintf(stderr, "\nUnknown command (%s) received from dealer. Disconnecting from game.\n", buffer);
            endGame(player_hand, dealer_hand, deck, decisions_made, client_fd);
            return -1;
        }
        recieve_message(buffer, sock);
    }

    endGame(player_hand, dealer_hand, deck, decisions_made, client_fd);

    return 0;
}

//*******************tokenizeInfo*******************//
//Parses info sent in from the dealer to know what action/response to take next
//Inputs: string (char*) input containing the line of information from the dealer, array to which to save each word in the line
//Outputs: an integer count of all words in the line
//
//Purpose: Splice info s.t. runGame can check the first word for a viable command, and use the rest of the information to conduct the necessary processing of the command
int tokenizeInfo(char* input, char** info_array){
    int wordNum = 0;
    bool inWord = false;
    int length = strlen(input);

    for (int i = 0; i < length; i++){
        //check if its a letter starting a word
        if (!isspace(input[i]) && !inWord){
            info_array[wordNum] = &input[i];
            wordNum++;
            inWord = true;
        //check if its the end of word
        }
        else if (inWord && isspace(input[i])){
            input[i] = '\0';
            inWord = false;
        }
    }

    return wordNum;
}

//*******************randomPick*******************//
//Chooses randomly between 1 and 0
//Inputs: none
//Outputs: returns the chosen integer
//
//Purpose: helper for training, to randomly decide between stand (1) and hit (0)
int randomPick(){
    srand(time(0));
    int num = rand() % 2;
    return num;
}

//*******************updateQ*******************//
//Updates Q and Q-count based on decisions made and outcomes reached during rounds played in training mode
//Inputs: a bag containing decision structures, an integer corresponding to the reward/game outcome
//Outputs: nothing, as the Q-count and Q table are updated directly
//
//Purpose: if in training mode, gets called to update the Q-table with the results of the different decisions made and the resulting game outcome (as 
//   well as the Q-count): helper for handleResult
void updateQ(bag_t* decisions, int reward) { 
    decision_t* d;
    while ((d = bag_extract(decisions)) != NULL){
        //get vals for Q table
        int player_points = d->player_score;
        int dealer_points = d->dealer_score;
        int action = d->action;
        int ace = d->aces;
        if (player_points <=20 && dealer_points <= 11){
            printf("%d, %d, %d, %d\n", player_points, dealer_points, action, ace);
            printf("reward: %d\n", reward);
            Q_count[player_points][dealer_points][action][ace] += 1; //increment count
            Q[player_points][dealer_points][action][ace] += (1/(float)Q_count[player_points][dealer_points][action][ace]) 
                                           * (reward - Q[player_points][dealer_points][action][ace]); //update reward

        }
        free(d);
    }
}

//*******************handleDecision*******************//
//Given a new card/having reached a decision point, reference the cards in hand and the known dealer card(s) to decide whether to hit or stand
//Inputs: boolean "training" to recognize whether the game is in training mode or if it's to be played knowledgably; integers for the player and dealer scores 
//   at that point of play, the bag of existing decisions, and the player's hand
//Outputs: an integer: 1 if stand, 0 if hit
//
//Purpose: Given the inputs, make a decision about whether to hit or stand: if training, make that choice at random and store it in the bag to later update Q: 
//   otherwise, reference the Q table to make the best decision with the given hands
int handleDecision(bool training, int total_score, int dealer_score, bag_t* decisions_made, hand_t* hand){
    if (total_score < 12){
        return 0;
    }
    if (total_score == 12  || total_score == 21 || (total_score > 21 && getNumAces(hand) <= 0)){
        return 1;
    }

// if the score goes over 21, check for an ace in hand (cards): if there is one, set ace to low, update aces in hand, and update player score by subtracting 10. 
    if (total_score > 21 && getNumAces(hand) > 0){
        setAceLow(hand);
        total_score = getCount(hand) + (11*(getNumAces(hand)));
        return handleDecision(training, total_score, dealer_score, decisions_made, hand);
    }

    if (training){
        //make a random decision
        int choice = randomPick();

        //keep track of decisions to later update Q
        decision_t* d = mem_malloc(sizeof(decision_t));
        d->action = choice;
        d->player_score = total_score;
        d->dealer_score = dealer_score;
        d->aces = getNumAces(hand);
        bag_insert(decisions_made, d);

        return choice;
    }

    else{
        if (Q[total_score][dealer_score][0][getNumAces(hand)]>Q[total_score][dealer_score][1][getNumAces(hand)]){ 
            return 0;
        }
        else{
            return 1;
        }
    }
}

//*******************handleResult*******************//
//Take in the string corresponding the game result and convert that to an integer: update the Q table if training, and regardless, print out the result
//Inputs: string (char*) "result" containing the result of the round, a boolean for training or not, and the bag of decisions
//Outputs: none
//
//Purpose: inform the user of the result of the game. if training, use the bag of decisions, the reward (as an integer), and updateQ to update that information 
//   to make a more informed decision moving forward.
void handleResult(char* result, bool training, bag_t* decisions){
    int reward;
    if (strcmp(result, "WIN")==0){
        reward = 1;
        fprintf(stdout, "\nThe player won!\n");
    }
    if (strcmp(result, "LOOSE")==0 || strcmp(result, "BUST")==0){
        reward = -1;
        fprintf(stdout, "\nThe player lost.\n");
    }
    if (strcmp(result, "PUSH")==0){
        reward = 0;
        fprintf(stdout, "\nThe player pushed.\n");
    }
    //if training, update the data
    if (training){
        //update the data
        updateQ(decisions, reward); 
    }
}

//*******************endGame*******************//
//Final cleanup after the game has been played or an error has occurred, to ensure that all game memory has been freed and all allocations are resolved.
//Inputs: the player and dealer hands, the deck, the bag of decisions (if not training, this should exist but just be empty), and the information for the network
//Outputs: none
//
//Purpose: Delete everything that needs to be deleted, free everything that needs to be freed, and close out the connection to the network
void endGame(hand_t* player_hand, hand_t* dealer_hand, deck_t* deck, bag_t* decisions_made, int client_fd){
    //free the bag of decisions
    bag_delete(decisions_made, decisionsDelete);
    //free the old p hand
    deleteHand(player_hand);
    //free the old d hand
    deleteHand(dealer_hand);
    //free the deck
    deleteDeck(deck);

    //close out the connection
    close_client(client_fd);
}

//*******************decisionsDelete*******************//
//Free memory allocated to decisions made s.t. the contents of the bag of decisions may be deleted
//Inputs: a decision made (taken from the bag of decisions)
//Outputs: none
//
//Purpose: helper to delete decisions within the bag of decisions. it's necessary for bag_delete to work, but also all decisions should be dealt with already and the bag 
void decisionsDelete(void* decision){
    if (decision != NULL) {
        free(decision);   
    }
}
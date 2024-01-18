/* 
 * dealer.c - dealer for BlackJack game
 *
 * The `dealer` is responsible for running the game of 21 from the server side. 
 * It allows players to connect, deals cards, sends messages to the player, 
 * receives messages from the player, and calculates the result of the game based 
 * on player and dealer point totals. It also considers edge cases, especially 
 * those involving Ace cards that can take on different values based on the 
 * value of other cards in the hand. The Dealer uses the deck and hand data 
 * structures defined in the cards module.
 *
 * Helen Liu, November 2022
 * CS 50, Fall 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cards.h"
#include "mem.h"
#include "network.h"

/******** local function prototypes *******/
static void parseArgs(const int argc, char* argv[], int* numGames, int* portNumber);
void dealerEvaluate(deck_t *deck, hand_t *dealerHand);
void playerEvaluate(deck_t *deck, hand_t *playerHand);
void handleAce(hand_t *hand, int *aceAddedHigh);
void calculateOutcome(hand_t *dealerHand, hand_t *playerHand, int new_socket);
void play(int new_socket, int numGames);

/******** main *******/
int main(int argc, char *argv[]) {
    int numGames = 0;
    int portNumber = 0;
    int server_fd, new_socket;
    //parse arguments from command line 
    parseArgs(argc, argv, &numGames, &portNumber);
    printf("port number: %d\n", portNumber);
    //start server for player to connect
    start_server(portNumber, &server_fd, &new_socket);
    //loop all game actions for number of games specified 
    play(new_socket, numGames);
    //shut down server
    shutdown_server(new_socket, server_fd);

    return 0;
}

/**************** parseArgs ****************/
/* parse arguments from command line
 */
static void parseArgs(const int argc, char* argv[], int* numGames, int* portNumber){
    //check number of args
    if (argc != 3) {
       printf("Invalid number of arguments\n");
       printf("Usage: %s numGames portNumber\n", argv[0]);
       exit(1);
    }
    //check numGames argument - integer & in range 
    char excess;
    if (sscanf(argv[1], "%d%c", numGames, &excess) != 1) {
        printf("Invalid numGames %s: numGames should be an integer \n", argv[1]);
        printf("Usage: %s numGames portNumber \n", argv[0]);
        exit(2);
    } else{
        if (*numGames < 1) {
            printf("Invalid numGames %s: numGames should be int > 0\n", argv[1]);
            printf("Usage: %s numGames portNumber \n", argv[0]);
            exit(3);
        }
    }
    //check portNumber argument - integer & in range 
    if (sscanf(argv[2], "%d%c", portNumber, &excess) != 1) {
        printf("Invalid portNumber %s: portNumber should be an integer \n", argv[2]);
        printf("Usage: %s numGames portNumber \n", argv[0]);
        exit(2);
    } else{
        if (*portNumber < 1) {
            printf("Invalid portNumber %s: portNumber should be int > 0\n", argv[2]);
            printf("Usage: %s numGames portNumber \n", argv[0]);
            exit(5);
        }
    }
}

/**************** dealerEvaluate ****************/
/* evaluate dealer's actions
 *
 * Once all players have taken cards until they STAND or BUST, if any players have not busted,
 * the dealer reveals with face-down card and then decides to HIT or STAND.
 *The dealer must continue to HIT if their total points are 16 or fewer. 
 */
void dealerEvaluate(deck_t *deck, hand_t *dealerHand){
    int value = 0;
    int *aceAddedHigh = &value;
    //if there are no aces in the dealer's starting hand
    if (getNumAces(dealerHand)==0){
        //dealer continues hitting while their total points are 16 or fewer
        while(getCount(dealerHand)+*aceAddedHigh<=16){
            hit(deck, dealerHand);
            //if dealer got an ace from this hit, handle it
            if(getNumAces(dealerHand)==1){
                handleAce(dealerHand, aceAddedHigh);
            }
        }
    //if there is at least 1 Ace in the dealer's starting hand, handle it
    } else {
        handleAce(dealerHand, aceAddedHigh);
        //keep hitting until dealer goes over 16 and has to STAND
        while(getCount(dealerHand)+*aceAddedHigh<=16){
            hit(deck, dealerHand);
            //if there is exactly one Ace in the hand now, handle it
            if(getNumAces(dealerHand)==1){
                handleAce(dealerHand, aceAddedHigh);
            }
        }
    }
    //if there was a temp Ace added, run dealerEvaluate again now that we have more info from hits
    //to properly set it high or low
    if(*aceAddedHigh == 11){
        dealerEvaluate(deck, dealerHand);
    }
}

/**************** playerEvaluate ****************/
/* take care of Aces in player's hand 
 * 
 * If there are more than 2 Aces, they will be set low 
 *
 * If there are 2 Aces, they will both be set low if having one high causes a bust
 * otherwise they will be set one low, one high
 *
 * If there is one Ace, if it causes a bust, it will be set low 
 * otherwise it will be set high
 */
void playerEvaluate(deck_t *deck, hand_t *playerHand){
    int numAces = getNumAces(playerHand);
    //if > 2 Aces, the extras must be set low so player doesn't bust 
    while (numAces > 2){
        setAceLow(playerHand);
    }
    //if there are 2 Aces, one Ace can be set high if 12 extra points (11+1) does not cause bust
    //otherwise set both low 
    if (numAces == 2){
        if (getCount(playerHand) + 12 > 21){
            setAceLow(playerHand);
            setAceLow(playerHand);
        } else {
            setAceLow(playerHand);
            setAceHigh(playerHand);
        }
    //if there is one Ace, set high if it does not cause bust 
    //otherwise set it low
    } else if (numAces == 1){
        if(getCount(playerHand)+11>21){
            setAceLow(playerHand);
        } else{
            setAceHigh(playerHand);
        }
    }
}

/**************** handleAce ****************/
/* handle an Ace in the dealer's hand
 * 
 * If dealer's total with Ace counting as 11 is over 16 but below 22:
 * Ace is set high and dealer will STAND.
 *
 * If dealer's total with Ace counting as 11 is over 22:
 * Ace is set low; temporary high ace is voided.
 *
 * If dealer's total with Ace counting as 11 is <= 16:
 * Temporary high ace is added to calculate point totals in dealerEvaluate 
 * in case the dealer busts later on & the ace needs to be set low
 */
void handleAce(hand_t *hand, int *aceAddedHigh){
    //if high ace lands dealer's points over 16 but under 22, dealer stands
    if (getCount(hand)+11 >16 && getCount(hand)+11 <22){
        setAceHigh(hand);
        //if there's a second ace in the hand, handle it 
        if(getNumAces(hand)>0){
            handleAce(hand, aceAddedHigh);
        }
    //if the dealer would BUST if ace is high, then set the ace low
    } else if(getCount(hand)+11 >=22){
        setAceLow(hand);
        //reset temp Ace value 
        if(*aceAddedHigh==11){
            *aceAddedHigh = 0;
        }
    //if the dealer's total score with a high ace is <= 16
    //try temporarily adding the ace high 
    //(will be set low if a later card causes player to bust)
    } else if (*aceAddedHigh == 0){
            *aceAddedHigh = *aceAddedHigh+11;
    }
}

/**************** calculateOutcome ****************/
/* calculate end result of the game & send message to player
 * 
 * RESULT WIN if player points > dealer points, both under 21 
 * or if dealer busts
 *
 * RESULT LOOSE if player points < dealer points, both under 21 
 * or if player busts
 * 
 * RESULT PUSH if points are tied under 21
 */
void calculateOutcome(hand_t *dealerHand, hand_t *playerHand, int new_socket){
    if ((getCount(playerHand)<=21 && getCount(playerHand)>getCount(dealerHand)) || (getCount(playerHand)<=21 && getCount(dealerHand)>21)){
        send_message("RESULT WIN", new_socket);
        printf("The Dealer lost.\n");
    } else if(getCount(playerHand)>21){
        send_message("RESULT LOOSE", new_socket);
        printf("The Dealer won.\n");
    } else if (getCount(playerHand)<getCount(dealerHand)){
        send_message("RESULT LOOSE", new_socket);
        printf("The Dealer won.\n");
    } else {
        send_message("RESULT PUSH", new_socket);
        printf("The Dealer and Player tied.\n");
    }
}

/**************** play ****************/
/* play the game! 
 * caller and server loop to keep listening for each other's messages
 */
void play(int new_socket, int numGames) {
    bool finish = false;
    char buffer[1024] = { 0 };
    while (! finish) {
        //receive join message from player
        recieve_message(buffer, new_socket);
        if(strncmp(buffer, "JOIN", 5)!=0){
            //send begin message
            send_message("BEGIN", new_socket);
            sleep(2);
            //initialize game, player, and dealer 
            for (int i=0; i<numGames; i++){
                deck_t *deck = initializeGame();
                hand_t *playerHand = initializePlayer(deck);
                hand_t *dealerHand = initializeDealer(deck);
                //deal 2 cards to the player & send message for each card
                for(int i=0; i<2; i++){
                    char CARD[30];
                    strcpy(CARD, "CARD ");
                    strcat(CARD, hit(deck, playerHand));
                    send_message(CARD, new_socket);
                    sleep(2);
                }
                //deal one card to dealer without communicating (face down) 
                hit(deck, dealerHand);
                //deal one card to dealer & send message (face up) 
                char CARD[40];
                strcpy(CARD, "DEALER ");
                strcat(CARD, hit(deck, dealerHand));
                send_message(CARD, new_socket);
                sleep(2);
                //send message to prompt player to make a decision
                send_message("DECISION", new_socket);
                sleep(2);
            
                //receive HIT/STAND decisions from the client
                char message[6];
                recieve_message(message, new_socket);
                //while the decision is not STAND & the player has not busted
                while(strcmp(message, "STAND") != 0 && getCount(playerHand)<=21){
                    //if the decision is HIT
                    if(strcmp(message, "HIT")==0){
                        //deal player another card & send message
                        char CARD[30];
                        strcpy(CARD, "CARD ");
                        strcat(CARD, hit(deck, playerHand));
                        send_message(CARD, new_socket);
                        sleep(2);
                        //prompt another decision
                        send_message("DECISION", new_socket);
                        sleep(2);
                        recieve_message(message, new_socket);
                    //if message is not HIT or STAND, send error message and try again 
                    } else{
                        send_message("Invalid Decision. Please HIT or STAND", new_socket);
                        sleep(2);
                        recieve_message(message, new_socket);
                    }
                }
                //when player has finished making decisions, evaluate player & dealer's hands 
                playerEvaluate(deck, playerHand);
                dealerEvaluate(deck, dealerHand);
                //print for dealer's reference 
                printf("Player's Final Score: %d\n", getCount(playerHand));
                printf("Dealer's Final Score: %d\n", getCount(dealerHand));
                //calculate the end result of game & send message to player
                calculateOutcome(dealerHand, playerHand, new_socket);
                //reset and play again
                deleteHand(dealerHand);
                deleteHand(playerHand);
                deleteDeck(deck);
                sleep(2);
            }
            //after the specified amount of games, quit 
            send_message("QUIT", new_socket);
            sleep(2);
        //quit if player does not send JOIN message
        } else{
            send_message("QUIT", new_socket);
            sleep(2);
        }
        finish = true;
    }
}
/* 
 * cards.h - header file for BlackJack cards module
 *
 * A deck of cards maintains a counter which is identified by a unique integer 0 to 52
 * and a count of how many times the car has been seen (1=unseen, 2=seen). It also
 * can convert the identifying number to the name of the card and vice versa using a
 * hashtable. Additionally there is a hand that keeps track of the name of the hand
 * "Dealer" or "Player", the count of their hand (not including aces), the number of
 * aces in their hand, and a bag of the names of the cards in their hand. The caller
 * can get cards from our deck or add cards to the hand. It can also access the info
 * stored in the hand.
 *
 * Julia Gottschalk, November 2022
 * CS 50, Fall 2022
 */

#ifndef CARDS_H
#define CARDS_H

#include "counters.h"
#include "bag.h"
#include "hashtable.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************** global types ****************/
typedef struct deck deck_t;
typedef struct hand hand_t;

/**************** functions ****************/
/*
To be called at beginning of game
We return a pointer to a deck, or NULL if error
Caller is later responsible for calling deleteDeck
*/
deck_t *initializeGame();

/*
Returns a pointer to the dealer's hand
Creates a bag that keeps track of the dealers hand
Keeps track of the dealer's name, the count of their hand, and the number of aces
If we are not using the dealerGiven, we will assign two cards. Only the first of which will be shown
Caller is later responsible for calling deleteHand
*/
hand_t *initializeDealer(deck_t *deck);

/*
Returns a pointer to the player's hand
Creates a bag that keeps track of the players hand
Keeps track of the player's name, the count of their hand, and the number of aces
If we are not using the dealerGiven, we will assign two cards. Both of which will be revealed
Caller is later responsible for calling deleteHand
*/
hand_t *initializePlayer(deck_t *deck);

/*
Called when player or dealer wants to receive a card.
Will add a card (that has not been dealt yet) to someones hand
Will update the count and numAces for that hand
Will print the updated hand
*/
char *hit(deck_t *deck, hand_t *hand);

/*
When using dealergiven this function can be used to add a card to a player's hand
It will also keep track in our deck that it has been used
It will update the players hand to include the new card and increase their count and numAces
*/
void addCardtoHand(deck_t *deck, hand_t *hand, char *card);

/*
Print the whole hand. 
We print: a comma-separated list of cards, surrounded by {brackets}.
 */
void printHand(hand_t *hand);

/*
Getter method to find how many aces someone has in their hand
*/
int getNumAces(hand_t *hand);

/*
Getter method to find the count of someones hand (does not include aces)
*/
int getCount(hand_t *hand);

/*
Can be called if player or dealer wants to set their ace low.
Decreases number of aces held in hand. Cannot be reversed
*/
void setAceLow(hand_t *hand);

/*
Can be called if player or dealer needs to set their ace high
Decreases number of aces held in hand. Cannot be reversed
*/
void setAceHigh(hand_t *hand);

/*
Frees the deck. Must provide a valid deck pointer
*/
void deleteDeck(deck_t *deck);

/*
Frees a hand. Must provide a valid hand pointer
*/
void deleteHand(hand_t *hand);


#endif // CARDS_H
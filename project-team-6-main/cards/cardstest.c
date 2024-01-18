/*
 * cardstest.c - test program for CS50 cards module
 *
 * input: none
 * output: none
 *
 * compile: make
 * usage: ./cardstest
 *
 * Julia Gottschaklk, November 2022
 * CS50, 22F
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cards.h"

int main() {
    //Test initializing game and player
    deck_t *deck = initializeGame();
    hand_t *dealerHand = initializeDealer(deck);
    hand_t *playerHand = initializePlayer(deck);
    //make sure prints all 52 cards
    for(int i=0; i<51; i++){
        char *string = hit(deck, playerHand);
        printf("%s\n", string);
    }
    //make sure deletes deck and hands properly
    deleteHand(dealerHand);
    deleteHand(playerHand);
    deleteDeck(deck);

    printf("\n\n");
    //create another deck and hands
    deck_t *deck2 = initializeGame();
    //check works for invalid deck
    hand_t *dealerHand2 = initializeDealer(NULL); 
    hand_t *playerHand2 = initializePlayer(NULL);
    //check with real deck
    dealerHand2 = initializeDealer(deck2); 
    playerHand2 = initializePlayer(deck2);

    //Test adding cards to hand based on a string
    printf("Count of Player's Hand: %d\n", getCount(playerHand2));
    addCardtoHand(deck2, playerHand2, "abcde"); //invalid card
    addCardtoHand(deck2, playerHand2, "Queen of Hearts");

    //ensure cannot set ace low or high if no aces
    printf("Number of Aces in Player's Hand: %d\n", getNumAces(playerHand2));
    setAceLow(playerHand2);
    setAceHigh(playerHand2);
    //check can properly set ace low or high if have aces
    addCardtoHand(deck2, playerHand2, "Ace of Spades");
    addCardtoHand(deck2, playerHand2, "Ace of Diamonds");
    printf("Number of Aces in Player's Hand: %d\n", getNumAces(playerHand2));
    printf("Count of Player's Hand: %d\n", getCount(playerHand2));
    setAceHigh(playerHand2);
    printf("Number of Aces in Player's Hand: %d\n", getNumAces(playerHand2));
    printf("Count of Player's Hand: %d\n", getCount(playerHand2));
    setAceLow(playerHand2);
    printf("New Count of Player's Hand: %d\n", getCount(playerHand2));
    printf("Count of Player's Hand: %d\n", getCount(playerHand2));


    addCardtoHand(deck2, dealerHand2, "Ace of Diamonds");
    addCardtoHand(deck2, dealerHand2, "Seven of Spades");
    //check can print hand properly
    printHand(dealerHand2);

    //check deletes everything properly (ensure with valgrind)
    deleteHand(dealerHand2);
    deleteHand(playerHand2);
    deleteDeck(deck2);
    return 0;
}
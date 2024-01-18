/*
 * card.c - BlackJack cards module
 *
 * see cards.h for more information.
 *
 * Julia Gottschalk, November 2022
 * CS 50, Fall 2022
 */
#include "counters.h"
#include "bag.h"
#include "hashtable.h"
#include "mem.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/**************** local types ****************/
//deck structure
typedef struct deck
{
    counters_t *cards; //card number to how many many times card has been seen (1=unseen, 2=seen)
    hashtable_t *LUT1; //number to string (i.e. "0": "Ace of Hearts")
    hashtable_t *LUT2; //string to number (i.e. "Ace of Hearts": "0")

} deck_t;

//hand structure
typedef struct hand
{
    bag_t *hand; //card names
    int count;  //count of hand besides aces
    int numAces; //num aces
    char *name; //name of hand (i.e. player or dealer)

} hand_t;

/**************** local functions ****************/
static void createCards(counters_t *cards);
static void createLUT(hashtable_t *LUT1, hashtable_t *LUT2);
static void addCount(hand_t *hand, int cardNum);
static char *getCard(deck_t *deck, hand_t *hand);
static void printBag(FILE* fp, void *item);
static void stringDelete(void* item);


/**************** global functions ****************/
//all global functions defined in cards.h
deck_t *initializeGame(){
    //allocate memory for deck
    deck_t *deck = mem_malloc(sizeof(deck_t));
    if (deck == NULL){
        return NULL;
    } else {
        deck->cards = counters_new(); //create counter
        if(deck->cards == NULL){ return NULL; }
        else { createCards(deck->cards); }
        deck->LUT1 = hashtable_new(52); //create Look up tables
        deck->LUT2 = hashtable_new(52);
        if(deck->LUT1 == NULL || deck->LUT2 == NULL){ return NULL; }

        else { createLUT(deck->LUT1, deck->LUT2); }
    }
    return deck;
}

//create counter for deck struct
static void createCards(counters_t *cards){
    //create a counter for each card number.
    //The count will be 1 initially, indicating it has not been seen
    for (int i=0; i<=51; i++){
        counters_add(cards,i);
    }
}

//create LUTs for deck struct
static void createLUT(hashtable_t *LUT1, hashtable_t *LUT2){
    for (int i=1; i<=13; i++){
        char *rank = NULL;
        //get rank based on num
        if (i==1){ rank = "Ace";
        } else if (i==2){ rank = "Two";
        } else if (i==3){ rank = "Three";
        } else if (i==4){ rank = "Four";
        } else if (i==5){ rank = "Five";
        } else if (i==6){ rank = "Six";
        } else if (i==7){ rank = "Seven";
        } else if (i==8){ rank = "Eight";
        } else if (i==9){ rank = "Nine";
        } else if (i==10){ rank = "Ten";
        } else if (i==11){ rank = "Jack";
        } else if (i==12){ rank = "Queen";
        } else{ rank = "King";
        }
        for (int j=1; j<=4; j++){
            char *suit = NULL;
            //get suit based on num
            if (j==1){ suit = "Hearts";
            } else if (j==2){ suit = "Spades";
            } else if (j==3){ suit = "Clubs";
            } else{ suit = "Diamonds";
            }

            //create a string that is "rank of suit"
            char *of = " of ";
            char *str = mem_malloc_assert(strlen(rank)+strlen(of)+strlen(suit)+1, "Failed to allocate memory for the cardID as a string\n");
            strcpy(str, rank);
            strcat(str, of);
            strcat(str, suit);

            //get the num of this card as a string
            int length = snprintf(NULL, 0, "%d", 4*(i-1)+(j-1));
            char *cardID = mem_malloc_assert(length +1, "Failed to allocate memory for the cardID as a string\n");
            snprintf(cardID, length+1,"%d", 4*(i-1)+(j-1));

            //insert into LUTs: Look up tables
            hashtable_insert(LUT1, cardID, str);
            hashtable_insert(LUT2, str, cardID);
        }
    }
}

hand_t *initializeDealer(deck_t *deck){
    if(deck!=NULL){
        hand_t *dealerHand = mem_malloc(sizeof(hand_t));
        //initialize components of the hand struct
        dealerHand->hand = bag_new();
        dealerHand->count = 0;
        dealerHand->numAces = 0;
        dealerHand->name = "Dealer";
        return dealerHand;
    } else{
        printf("Invalid deck\n");
        return NULL;
    }
}

hand_t *initializePlayer(deck_t *deck){
    if(deck!=NULL){
        hand_t *playerHand = mem_malloc(sizeof(hand_t));
        //initialize components of the hand struct
        playerHand->hand = bag_new();
        playerHand->count = 0;
        playerHand->numAces = 0;
        playerHand->name = "Player";
        return playerHand;
    } else{
        printf("Invalid deck\n");
        return NULL;
    }

}

//increase count when dealt a card
static void addCount(hand_t *hand, int cardNum){
    int rank = (cardNum/4) %13 +1;
    if (rank > 10){
        rank = 10; //if greater than 10, must be jack, queen, king = 10
    }
    if (rank==1){
        hand->numAces = hand->numAces + 1; //if one just increment numAces
    } else {
        hand->count = hand->count + rank; //if not one then updateCount
    }
}

//get a card from the deck
static char *getCard(deck_t *deck, hand_t *hand){
    srand(time(0));
    //choose a random card
    int cardNum = rand() % 52;
    while (counters_get(deck->cards, cardNum)!=1){
        cardNum = rand() % 52;
    }
    counters_set(deck->cards, cardNum, 2); //mark card as having been dealt
    int length = snprintf(NULL, 0, "%d", cardNum);
    char *cardID = mem_malloc_assert(length +1, "Failed to allocate memory for the cardID as a string\n");
    snprintf(cardID, length+1,"%d", cardNum);
    char *card = malloc(strlen(hashtable_find(deck->LUT1, cardID))+1);
    //get card as actual name
    strcpy(card, hashtable_find(deck->LUT1, cardID));
    //update player or dealer's hand accordingly
    addCount(hand, cardNum);
    bag_insert(hand->hand, card);
    mem_free(cardID);
    return card;
}

char *hit(deck_t *deck, hand_t *hand){
    return getCard(deck, hand); //get card
}

void printHand(hand_t *hand){
   bag_print(hand->hand, stdout, printBag);
   fprintf(stdout, "\n");
}

int getCount(hand_t *hand){
    return hand->count;
}

int getNumAces(hand_t *hand){
    return hand->numAces;
}

void setAceLow(hand_t *hand){
    if(getNumAces(hand)==0){
        fprintf(stdout, "You tried to set the NumAces low when you had no aces.\n");
    } else{
        hand->numAces = hand->numAces-1; //decrease number of aces
        hand->count = hand->count + 1; //set the count as updated value
    }
}

void setAceHigh(hand_t *hand){
    if(getNumAces(hand)==0){
        fprintf(stdout, "You tried to set the NumAces high when you had no aces.\n");
    } else{
        hand->count = hand->count+11; //set the count as updated value
        hand->numAces = hand->numAces-1; //decrease number of aces
    }
}

void deleteDeck(deck_t *deck){
    //free all modules
    counters_delete(deck->cards);
    hashtable_delete(deck->LUT1, stringDelete);
    hashtable_delete(deck->LUT2, stringDelete);
    mem_free(deck);
}

void deleteHand(hand_t *hand){
    //free the bag
    bag_delete(hand->hand, stringDelete);
    mem_free(hand);
}

void addCardtoHand(deck_t *deck, hand_t *hand, char *card){
    //allocate memory for the cardID and copy it in
    if(hashtable_find(deck->LUT2, card) != NULL){
        char *cardID = malloc(strlen(hashtable_find(deck->LUT2, card))+1);
        strcpy(cardID, hashtable_find(deck->LUT2, card));
        int cardNum = atoi(cardID); //get the cardID as an integer

        counters_set(deck->cards, cardNum, 2); //report that the card was seen
        addCount(hand, cardNum); //update the count of the hand

        //add card to hand and print the updated hand
        char *card1 = malloc(strlen(card)+1);
        strcpy(card1, card);
        bag_insert(hand->hand, card1);
        mem_free(cardID);
    } else {
        printf("Invalid Card Name\n");
    }
}

//print the pag module, used to print a hand of cards
static void printBag(FILE* fp, void *item){
    //print the item (which is a string)
	fprintf(fp, "%s", (char *)item);
}

//delete a string, used to delete items in the bag and hashtable
static void stringDelete(void* item){
    //if the item is not null, free it
    if (item != NULL) {
        mem_free(item);   
    }
}
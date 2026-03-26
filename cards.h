//
// Created by xleve on 3/25/2026.
//

#ifndef POKER_TP_CARDS_H
#define POKER_TP_CARDS_H

typedef enum {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
}Suit;

typedef enum {
    TWO = 2, THREE, FOUR, FIVE, SIX,
    SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
}Rank;

typedef struct {
    Rank rank;
    Suit suit;
}Card;

#define DECK_SIZE 52

typedef struct {
    Card cards[DECK_SIZE];
    int top; // urmatoarea carte
}Deck;

#define MAX_PLAYERS 6

typedef struct {
    Card hand[2];
} Player;

//deck

void InitDeck(Deck *deck);
void RandomizeDeck(Deck *deck);
void BogoShuffle(Deck *deck,int MaxAttempts);

//print

const char* rankToString(int rank);
const char* suitToString(int suit);
void PrintHand(Player player, int PlayerNumber);

//deal

void DealStartingHands(Deck *deck, Player players[], int numPlayers);
Card DrawCard(Deck *deck);


#endif //POKER_TP_CARDS_H


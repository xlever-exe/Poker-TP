#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cards.h"


Card DrawCard(Deck *deck) {

    if (deck->top < DECK_SIZE) {
        Card drawnCard = deck->cards[deck->top]; // Get the top card
        deck->top++;                             // Move to the next card
        return drawnCard;
    } else {
        printf("Error: Deck is empty. Cannot draw more cards.\n");
        Card errorCard = { (Rank)0, (Suit)0 }; // Returns an invalid card
        return errorCard;
    }
}

void DealStartingHands(Deck *deck, Player players[], int numPlayers) {
    printf("Dealing starting hands for %d players...\n\n", numPlayers);

    for (int cardIndex = 0; cardIndex < 2; cardIndex++) {

        for (int p = 0; p < numPlayers; p++) {
            players[p].hand[cardIndex] = DrawCard(deck);
        }

    }
}

int main (void) {
    srand(time(NULL));
    Player players[MAX_PLAYERS];
    Deck deck;
    InitDeck(&deck);
    BogoShuffle(&deck, rand () % DECK_SIZE);
    DealStartingHands(&deck, players, MAX_PLAYERS);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        PrintHand(players[i], i+1);
        printf("---\n");
    }

    return 0;
}
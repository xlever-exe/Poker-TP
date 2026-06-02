#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cards.h"
#include "raylib.h"

void BurnCard(Deck *deck) {
    if (deck->top < DECK_SIZE) {
        printf("Burning a card...\n");
        DrawCard(deck);
    }
}

void DealCommunityCards(Deck *deck, Card communityCards[], int numCards) {
    for(int i = 0; i < numCards; i++) {
        communityCards[i] = DrawCard(deck);
    }
}

Card DrawCard(Deck *deck) {

    if (deck->top < DECK_SIZE) {
        Card drawnCard = deck->cards[deck->top];
        deck->top++;
        return drawnCard;
    } else {
        printf("Error: Deck is empty. Cannot draw more cards.\n");
        Card errorCard = { (Rank)0, (Suit)0 };
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


#include <stdlib.h>
#include <stdio.h>

#include "cards.h"

void InitDeck(Deck *deck) {
    int index = 0;

    for (int suit = HEARTS; suit <= SPADES; suit++) {
        for (int rank = TWO; rank <= ACE; rank++) {
            deck->cards[index].rank = rank;
            deck->cards[index].suit = suit;
            index++;
        }
    }

    deck->top = 0;
}
void RandomizeDeck(Deck *deck) {
    for (int i = 0; i < DECK_SIZE; i++) {
        int j = rand() % DECK_SIZE;

        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}
void BogoShuffle(Deck *deck,int MaxAttempts) { // un fel de bogosort ( nu prea are rost sa verific daca e sortat deckul )
    int attempts = 0;
    while (attempts < MaxAttempts) {
        RandomizeDeck(deck);
        attempts++;
    }

}

const char* suitToString(int suit) {
    switch (suit) {
        case HEARTS: return "HEARTS";
        case DIAMONDS: return "DIAMONDS";
        case CLUBS: return "CLUBS";
        case SPADES: return "SPADES";
        default: return "UNKNOWN";
    }
}

const char* rankToString(int rank) {
    switch (rank) {
        case TWO: return "TWO";
        case THREE: return "THREE";
        case FOUR: return "FOUR";
        case FIVE: return "FIVE";
        case SIX: return "SIX";
        case SEVEN: return "SEVEN";
        case EIGHT: return "EIGHT";
        case NINE: return "NINE";
        case TEN: return "TEN";
        case JACK: return "JACK";
        case QUEEN: return "QUEEN";
        case KING: return "KING";
        case ACE: return "ACE";
        default: return "UNKNOWN";
    }
}
void PrintHand(Player player, int PlayerNumber) {
    printf("Player %d's Hand: [%s of %s] | [%s of %s]\n",
    PlayerNumber,
    rankToString(player.hand[0].rank),
    suitToString(player.hand[0].suit),
    rankToString(player.hand[1].rank),
    suitToString(player.hand[1].suit));
}








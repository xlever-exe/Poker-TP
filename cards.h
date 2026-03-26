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

void InitDeck(Deck *deck);
void ShuffleDeck(Deck *deck);
Card DrawCard(Deck *deck);

#endif //POKER_TP_CARDS_H


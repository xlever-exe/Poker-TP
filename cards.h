//
// Created by xleve on 3/25/2026.
//

#ifndef POKER_TP_CARDS_H
#define POKER_TP_CARDS_H

// ==========================================
// 1. BASIC CARD DEFINITIONS
// ==========================================
typedef enum {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
} Suit;

typedef enum {
    TWO = 2, THREE, FOUR, FIVE, SIX,
    SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
} Rank;

typedef struct {
    Rank rank;
    Suit suit;
} Card;

#define DECK_SIZE 52

typedef struct {
    Card cards[DECK_SIZE];
    int top; // urmatoarea carte
} Deck;


// ==========================================
// 2. GAME STATE & PLAYER STRUCTURES
// ==========================================
#define MAX_PLAYERS 6

typedef struct {
    Card hand[2];
    int chips;           // Total chips the player holds
    int currentBet;      // How much the player has bet in the *current* round
    int hasFolded;       // 1 if folded, 0 if still in the hand
    int isAllIn;         // 1 if all in, 0 otherwise
} Player;

typedef struct {
    int pot;                 // Total chips in the center
    int currentHighestBet;   // The amount needed to stay in (to call)
    int activePlayers;       // How many players haven't folded
} GameState;


// ==========================================
// 3. HAND EVALUATION STRUCTURES
// ==========================================
typedef enum {
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH
} HandCategory;

typedef struct {
    HandCategory category;
    int values[5]; // Stores [Primary, Secondary, Kicker1, Kicker2, Kicker3]
} HandScore;


// ==========================================
// 4. FUNCTION PROTOTYPES
// ==========================================

// Deck Management (cards.c)
void InitDeck(Deck *deck);
void RandomizeDeck(Deck *deck);
void BogoShuffle(Deck *deck, int MaxAttempts);

// Printing Utilities (cards.c)
const char* rankToString(int rank);
const char* suitToString(int suit);
void PrintHand(Player player, int PlayerNumber);

// Dealing Mechanics (dealcards.c)
void DealStartingHands(Deck *deck, Player players[], int numPlayers);
Card DrawCard(Deck *deck);
void BurnCard(Deck *deck); // The new burn card function

// Game & Player Management (cards.c / game.c)
void InitPlayers(Player players[], int numPlayers, int startingChips);
void ResetBettingRound(Player players[], int numPlayers, GameState *state);
void ConductBettingRound(Player players[], int numPlayers, GameState *state);

// Hand Evaluation (logic.c)
HandScore EvaluateHand(Card hole[2], Card community[5]);
int CompareHands(HandScore a, HandScore b);

#endif //POKER_TP_CARDS_H


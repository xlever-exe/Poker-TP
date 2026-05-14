#include "cards.h"
#include <stdio.h>

// Initialize players with chips
void InitPlayers(Player players[], int numPlayers, int startingChips) {
    for (int i = 0; i < numPlayers; i++) {
        players[i].chips = startingChips;
        players[i].currentBet = 0;
        players[i].hasFolded = 0;
        players[i].isAllIn = 0;
    }
}

// Resets bets and pot for a new round of betting (e.g., Flop -> Turn)
void ResetBettingRound(Player players[], int numPlayers, GameState *state) {
    state->currentHighestBet = 0;
    for (int i = 0; i < numPlayers; i++) {
        players[i].currentBet = 0;
    }
}

// The core betting loop
void ConductBettingRound(Player players[], int numPlayers, GameState *state) {
    int bettingActive = 1;

    while (bettingActive) {
        bettingActive = 0; // Assume we are done unless someone raises

        for (int i = 0; i < numPlayers; i++) {
            // Skip players who folded or are all-in
            if (players[i].hasFolded || players[i].isAllIn) continue;

            // If the player hasn't matched the current bet, they MUST act
            if (players[i].currentBet < state->currentHighestBet || state->currentHighestBet == 0) {

                printf("\n--- Player %d's Turn ---\n", i + 1);
                printf("Pot: %d | To Call: %d | Your Chips: %d\n",
                       state->pot,
                       state->currentHighestBet - players[i].currentBet,
                       players[i].chips);

                // --- PLACEHOLDER FOR USER INPUT ---
                // For now, we will simulate a simple AI: Always Call.
                // Later, you will add `scanf` here to ask: 1. Fold 2. Call 3. Raise

                int amountToCall = state->currentHighestBet - players[i].currentBet;

                if (players[i].chips >= amountToCall) {
                    printf("Player %d CALLS %d chips.\n", i + 1, amountToCall);
                    players[i].chips -= amountToCall;
                    players[i].currentBet += amountToCall;
                    state->pot += amountToCall;
                } else {
                    // All-in logic goes here
                    printf("Player %d FOLDS (Not enough chips to call).\n", i + 1);
                    players[i].hasFolded = 1;
                    state->activePlayers--;
                }

                // If someone raised here, you would set:
                // state->currentHighestBet = newTotalBet;
                // bettingActive = 1; // This forces the while-loop to run again so others can respond to the raise!
            }
        }
    }
    printf("\nBetting round complete. Total Pot: %d\n", state->pot);
}

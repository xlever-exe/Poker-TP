#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cards.h"
#include "raylib.h"


//test1
// Screen dimensions
const int screenWidth = 1280;
const int screenHeight = 720;

void BurnCard(Deck *deck) {
    if (deck->top < DECK_SIZE) {
        printf("Burning a card...\n");
        DrawCard(deck); // We draw the card but don't save it anywhere
    }
}

void DealCommunityCards(Deck *deck, Card communityCards[], int numCards) {
    for(int i = 0; i < numCards; i++) {
        communityCards[i] = DrawCard(deck);
    }
}

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

int main(void) {
    // ==========================================
    // 1. INITIALIZE WINDOW & ASSETS
    // ==========================================
    InitWindow(screenWidth, screenHeight, "Texas Hold'em - Raylib");
    SetTargetFPS(60);

    // Load the sprite sheet (Make sure cards.png is in cmake-build-debug)
    Texture2D spriteSheet = LoadTexture("cards.png");

    // ==========================================
    // 2. INITIALIZE GAME STATE
    // ==========================================
    srand(time(NULL));
    Deck deck;
    Player players[MAX_PLAYERS];

    // Pot=0, CurrentBet=0, ActivePlayers=MAX_PLAYERS
    GameState state = {0, 0, MAX_PLAYERS};

    InitDeck(&deck);
    BogoShuffle(&deck, rand() % DECK_SIZE + 50);
    InitPlayers(players, MAX_PLAYERS, 1000);
    DealStartingHands(&deck, players, MAX_PLAYERS);

    // ==========================================
    // 3. THE CORE GAME LOOP
    // ==========================================
    while (!WindowShouldClose()) {

        // --- DRAWING LOGIC (Visuals) ---
        BeginDrawing();

        // Draw the poker table
        ClearBackground(DARKGREEN);

        // Draw Title & Pot
        DrawText("Texas Hold'em", 20, 20, 40, LIGHTGRAY);

        char potText[50];
        sprintf(potText, "POT: $%d", state.pot);
        DrawText(potText, screenWidth/2 - 50, screenHeight/2 - 100, 30, YELLOW);

        // Draw Player 1's Text
        DrawText("Your Hand:", screenWidth/2 - 50, screenHeight - 250, 20, WHITE);

        // --- DRAW PLAYER 1'S CARDS ---
        for (int i = 0; i < 2; i++) {
            Card c = players[0].hand[i];

            // Find the correct column for the rank
            int colIndex;
            if (c.rank == ACE) colIndex = 1;
            else colIndex = c.rank;

            // --- SPRITE SHEET MEASUREMENTS ---
            // Tweak these if the framing is still slightly off!
            float startOffsetX = 5.0f;
            float startOffsetY = 5.0f;
            float actualCardWidth = 42.0f;
            float actualCardHeight = 60.0f;
            float gapX = 4.0f;
            float gapY = 4.0f;

            // Calculate pixel coordinates based on gaps and sizes
            float sourceX = startOffsetX + (colIndex * (actualCardWidth + gapX));
            float sourceY = startOffsetY + (c.suit * (actualCardHeight + gapY));

            Rectangle sourceRec = { sourceX, sourceY, actualCardWidth, actualCardHeight };

            // Scale the cards up by 2.5x so they look nice and big
            float scale = 2.5f;

            // Position them side-by-side at the bottom center
            Vector2 position = {
                (screenWidth/2 - (actualCardWidth*scale) - 10) + (i * ((actualCardWidth*scale) + 20)),
                screenHeight - 200
            };

            // Draw it!
            Rectangle destRec = { position.x, position.y, actualCardWidth * scale, actualCardHeight * scale };
            Vector2 origin = { 0, 0 };

            DrawTexturePro(spriteSheet, sourceRec, destRec, origin, 0.0f, WHITE);
        }

        EndDrawing();
    }

    // ==========================================
    // 4. CLEANUP
    // ==========================================
    UnloadTexture(spriteSheet); // Free the image from the graphics card
    CloseWindow();              // Close the display window

    return 0;
}
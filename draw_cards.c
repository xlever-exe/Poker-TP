#include "raylib.h"
#include "cards.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

const int screenWidth = 1280;
const int screenHeight = 720;

// ECONOMIE
int LoadChips() {
    FILE *file = fopen("save.txt", "r");
    if (file) {
        int c;
        fscanf(file, "%d", &c);
        fclose(file);
        if (c <= 0) return 1000; // Dacă ești falit complet, primești 1000$ "din partea casei" la restart
        return c;
    }
    return 1000; // Cont nou
}

void SaveChips(int c) {
    FILE *file = fopen("save.txt", "w");
    if (file) {
        fprintf(file, "%d", c);
        fclose(file);
    }
}

//  BOTI
int DecideBotAction(Player bot, GameState state, Card communityCards[5]) {
    int amountToCall = state.currentHighestBet - bot.currentBet;
    HandScore score = EvaluateHand(bot.hand, communityCards);

    // PRE-FLOP
    if (state.phase == PHASE_PREFLOP) {
        // Cărți premium (Valeți+): Dau Raise
        if (bot.hand[0].rank >= JACK && bot.hand[1].rank >= JACK) return 2;

        // Orice pereche mică
        if (bot.hand[0].rank == bot.hand[1].rank) {
            if (amountToCall > bot.chips / 3) return 1; // E prea scump să dea Raise, se mulțumesc cu Call
            return 2;
        }

        // Dacă cineva a pariat mai mult de 25% din bugetul botului, dă fold la cărți slabe
        if (amountToCall > bot.chips / 4) return 0;

        if (amountToCall == 0) return 1;
        if (rand() % 100 < 15) return 1;  // 15% șansă să dea call la noroc
        return 0; // FOLD
    }

    // POST-FLOP
    if (score.category >= THREE_OF_A_KIND) return 2; // Mână puternică -> RAISE

    if (score.category >= ONE_PAIR) {
        // Nu dă call la pariuri nesimțite (peste 33% din banii lui) cu doar o pereche
        if (amountToCall > bot.chips / 3) return 0;
        return 1; // CALL
    }

    if (amountToCall == 0) return 1; // Check gratis
    if (rand() % 100 < 5) return 2;  // Bluff foarte rar (5%)
    return 0; // FOLD
}

// ========================================================
// FUNCȚII PENTRU GRAFICĂ ȘI UI
// ========================================================

void DrawSpriteCard(Card c, Vector2 position, float scale, Texture2D spriteSheet) {
    if (c.rank == 0) return;


    int colIndex;
    if (c.rank == ACE) {
        colIndex = 0;
    } else {
        colIndex = c.rank - 1;
    }


    int rowIndex = 0;
    switch (c.suit) {
        case CLUBS:    rowIndex = 0; break; // Rândul 1 e Treflă
        case DIAMONDS: rowIndex = 1; break; // Rândul 2 e Romb
        case HEARTS:   rowIndex = 2; break; // Rândul 3 e Inimă Roșie
        case SPADES:   rowIndex = 3; break; // Rândul 4 e Inimă Neagră
    }


    float actualCardWidth = 167.5f;
    float actualCardHeight = 239.0f;


    float gapX = 0.0f;
    float gapY = 5.2f;


    float startOffsetX = 0.0f;
    float startOffsetY = 0.0f;

    float sourceX = startOffsetX + (colIndex * (actualCardWidth + gapX));
    float sourceY = startOffsetY + (rowIndex * (actualCardHeight + gapY));

    Rectangle sourceRec = { sourceX, sourceY, actualCardWidth, actualCardHeight };
    Rectangle destRec = { position.x, position.y, actualCardWidth * scale, actualCardHeight * scale };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(spriteSheet, sourceRec, destRec, origin, 0.0f, WHITE);
}
void DrawCardBack(Vector2 position, float scale, Texture2D spriteSheet) {
    float actualCardWidth = 167.5f;
    float actualCardHeight = 239.0f;
    float gapY = 13.0f;


    int backColumnIndex = 2;


    float fineTuneY = -35.0f;

    float sourceX = backColumnIndex * actualCardWidth;
    float sourceY = 4 * (actualCardHeight + gapY) + fineTuneY; // Am adăugat reglajul aici

    Rectangle sourceRec = { sourceX, sourceY, actualCardWidth, actualCardHeight };
    Rectangle destRec = { position.x, position.y, actualCardWidth * scale, actualCardHeight * scale };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(spriteSheet, sourceRec, destRec, origin, 0.0f, WHITE);
}

// 2. Structură și funcție pentru Butoane
typedef struct {
    Rectangle rect;
    const char* text;
    Color color;
} Button;

bool DrawAndCheckButton(Button btn) {
    Vector2 mousePoint = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePoint, btn.rect);
    bool isClicked = false;

    // Efect de hover
    if (isHovered) {
        DrawRectangleRec(btn.rect, LIGHTGRAY);
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isClicked = true;
        }
    } else {
        DrawRectangleRec(btn.rect, btn.color);
    }

    // Contur negru
    DrawRectangleLinesEx(btn.rect, 2.0f, BLACK);

    // Centrare text
    int textWidth = MeasureText(btn.text, 20);
    int textX = (int)(btn.rect.x + (btn.rect.width / 2) - (textWidth / 2));
    int textY = (int)(btn.rect.y + 15);
    DrawText(btn.text, textX, textY, 20, BLACK);

    return isClicked;
}
const char* GetHandName(int category) {
    switch (category) {
        case 0: return "High Card (Carte Mare)";
            // ATENȚIE: Folosesc numele standard din engleză. Dacă în cards.h ai alte nume (ex: PAIR, FLUSH), schimbă-le mai jos!
        case 1: return "O Pereche";          // ONE_PAIR
        case 2: return "Doua Perechi";       // TWO_PAIR
        case 3: return "Trei de un fel";     // THREE_OF_A_KIND
        case 4: return "Chinta (Straight)";  // STRAIGHT
        case 5: return "Culoare (Flush)";    // FLUSH
        case 6: return "Full House";         // FULL_HOUSE
        case 7: return "Careu (Quads)";      // FOUR_OF_A_KIND
        case 8: return "Chinta la Culoare";  // STRAIGHT_FLUSH
        default: return "Mana Necunoscuta";
    }
}



int main(void) {
    InitWindow(screenWidth, screenHeight, "Texas Hold'em - Modern UI");
    InitAudioDevice();
    SetTargetFPS(60);

    // Încărcăm Grafica și Sunetele
    Texture2D spriteSheet = LoadTexture("cards.png");
    Texture2D bgMenu = LoadTexture("casino_bg.png");
    Texture2D tableBg = LoadTexture("table_bg.png");
    Sound sndClick = LoadSound("click.mp3");
    Sound sndChips = LoadSound("chips.mp3");
    Sound sndFold = LoadSound("fold.mp3");
    Sound sndWin = LoadSound("win.mp3");

    srand(time(NULL));
    Deck deck;
    int activeNumPlayers = 4;
    Player players[4];
    Card communityCards[5] = {0};

    GameState state = {0, 0, activeNumPlayers, PHASE_PREFLOP};
    int winnerIndex = -1;
    int winningCategory = 0;

    int currentTurn = 0;
    int hasActed[4] = {0};
    char actionText[4][50] = {0};
    double actionTimer = 0.0;
    bool actionJustTaken = false;

    bool isTypingRaise = false;
    int raiseAmount = 0;
    bool inMainMenu = true;

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        // MENIU PRINCIPAL
        if (inMainMenu) {
            BeginDrawing();
            ClearBackground(BLACK);

            float offsetX = (mousePoint.x - (screenWidth / 2.0f)) * 0.03f;
            float offsetY = (mousePoint.y - (screenHeight / 2.0f)) * 0.03f;
            Rectangle sourceRec = { 0.0f, 0.0f, (float)bgMenu.width, (float)bgMenu.height };
            Rectangle destRec = { -50.0f - offsetX, -50.0f - offsetY, screenWidth + 100.0f, screenHeight + 100.0f };
            DrawTexturePro(bgMenu, sourceRec, destRec, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

            DrawText("TEXAS HOLD'EM", screenWidth/2 - MeasureText("TEXAS HOLD'EM", 70)/2, 200, 70, GOLD);

            char saveText[50];
            sprintf(saveText, "Fonduri salvate: $%d", LoadChips());
            DrawText(saveText, screenWidth/2 - MeasureText(saveText, 25)/2, 290, 25, GREEN);

            Button btnStart = { { screenWidth/2 - 125, 420, 250, 70 }, "START JOC", (Color){ 46, 204, 113, 255 } }; // Verde modern
            if (DrawAndCheckButton(btnStart)) {
                PlaySound(sndClick);

                InitDeck(&deck);
                BogoShuffle(&deck, rand() % DECK_SIZE + 50);
                InitPlayers(players, activeNumPlayers, 1000);
                players[0].chips = LoadChips();
                DealStartingHands(&deck, players, activeNumPlayers);

                for(int i=0; i<4; i++) { hasActed[i] = 0; actionText[i][0] = '\0'; }

                if (players[1].chips >= 10) { players[1].chips -= 10; players[1].currentBet = 10; }
                else { players[1].currentBet = players[1].chips; players[1].chips = 0; players[1].isAllIn = 1; }

                if (players[2].chips >= 20) { players[2].chips -= 20; players[2].currentBet = 20; }
                else { players[2].currentBet = players[2].chips; players[2].chips = 0; players[2].isAllIn = 1; }

                state.pot = players[1].currentBet + players[2].currentBet;
                state.currentHighestBet = 20;
                state.phase = PHASE_PREFLOP;
                winnerIndex = -1;

                currentTurn = 3;
                actionTimer = GetTime();

                inMainMenu = false;
            }

            EndDrawing();
            continue;
        }

        // LOGICA JOC
        int playerToCall = state.currentHighestBet - players[0].currentBet;
        bool isForcedAllIn = false;
        char callBtnText[50];

        if (playerToCall == 0) sprintf(callBtnText, "CHECK");
        else if (playerToCall >= players[0].chips) { sprintf(callBtnText, "CALL (ALL-IN)"); isForcedAllIn = true; }
        else sprintf(callBtnText, "CALL ($%d)", playerToCall);

        // GRAFICA BUTOANE
        Button btnFold  = { { screenWidth - 550, screenHeight - 90, 110, 50 }, "FOLD", (Color){ 231, 76, 60, 255 } }; // Rosu plat
        Button btnCall  = { { screenWidth - 425, screenHeight - 90, 160, 50 }, callBtnText, (Color){ 46, 204, 113, 255 } }; // Verde plat
        Button btnRaise = { { screenWidth - 250, screenHeight - 90, 120, 50 }, "RAISE", (Color){ 52, 152, 219, 255 } }; // Albastru plat
        Button btnAllIn = { { screenWidth - 115, screenHeight - 90, 90, 50 }, "MAX", (Color){ 155, 89, 182, 255 } }; // Mov plat
        Button btnReset = { { screenWidth/2 - 100, screenHeight - 90, 200, 50 }, "Urmatoarea Mana", (Color){ 230, 126, 34, 255 } };

        if (state.phase != PHASE_SHOWDOWN) {
            if (currentTurn == 0 && !players[0].hasFolded && !players[0].isAllIn) {
                if (!isTypingRaise) {
                    if (CheckCollisionPointRec(mousePoint, btnCall.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        int amount = isForcedAllIn ? players[0].chips : playerToCall;
                        players[0].chips -= amount; players[0].currentBet += amount; state.pot += amount;
                        if (isForcedAllIn) players[0].isAllIn = 1;

                        hasActed[0] = 1; actionJustTaken = true;
                        sprintf(actionText[0], amount == 0 ? "CHECK" : (isForcedAllIn ? "ALL-IN" : "CALL $%d"), amount);
                        PlaySound(amount == 0 ? sndClick : sndChips);
                    }
                    else if (CheckCollisionPointRec(mousePoint, btnFold.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        players[0].hasFolded = 1; hasActed[0] = 1; actionJustTaken = true;
                        sprintf(actionText[0], "FOLD"); PlaySound(sndFold);
                    }
                    else if (CheckCollisionPointRec(mousePoint, btnAllIn.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        int amount = players[0].chips;
                        players[0].chips = 0; players[0].currentBet += amount; state.pot += amount; players[0].isAllIn = 1;
                        if (players[0].currentBet > state.currentHighestBet) {
                            state.currentHighestBet = players[0].currentBet;
                            for(int i=1; i<4; i++) hasActed[i] = 0;
                        }
                        hasActed[0] = 1; actionJustTaken = true;
                        sprintf(actionText[0], "ALL-IN!"); PlaySound(sndChips);
                    }
                    else if (!isForcedAllIn && CheckCollisionPointRec(mousePoint, btnRaise.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        PlaySound(sndClick); isTypingRaise = true; raiseAmount = 0;
                    }
                } else {
                    int key = GetCharPressed();
                    while (key > 0) { if ((key >= '0') && (key <= '9')) raiseAmount = raiseAmount * 10 + (key - '0'); key = GetCharPressed(); }
                    if (IsKeyPressed(KEY_BACKSPACE)) raiseAmount /= 10;
                    if (IsKeyPressed(KEY_ENTER)) {
                        int totalAdd = playerToCall + raiseAmount;
                        if (totalAdd >= players[0].chips) {
                            totalAdd = players[0].chips; players[0].isAllIn = 1; sprintf(actionText[0], "ALL-IN");
                        } else sprintf(actionText[0], "RAISE $%d", raiseAmount);

                        players[0].chips -= totalAdd; players[0].currentBet += totalAdd; state.pot += totalAdd;
                        state.currentHighestBet = players[0].currentBet;

                        for(int i=1; i<4; i++) hasActed[i] = 0;
                        hasActed[0] = 1; actionJustTaken = true; isTypingRaise = false; PlaySound(sndChips);
                    }
                    if (IsKeyPressed(KEY_ESCAPE)) { isTypingRaise = false; PlaySound(sndClick); }
                }
            }
            else if (currentTurn > 0 && !players[currentTurn].hasFolded && !players[currentTurn].isAllIn) {
                if (GetTime() - actionTimer > 1.5) {
                    int p = currentTurn;
                    int botToCall = state.currentHighestBet - players[p].currentBet;
                    int action = DecideBotAction(players[p], state, communityCards);

                    if (action == 0 && botToCall > 0) {
                        players[p].hasFolded = 1; sprintf(actionText[p], "FOLD"); PlaySound(sndFold);
                    } else if (action == 1 || (action == 0 && botToCall == 0)) {
                        if (botToCall >= players[p].chips) {
                            botToCall = players[p].chips; players[p].isAllIn = 1; sprintf(actionText[p], "ALL-IN");
                        } else sprintf(actionText[p], botToCall == 0 ? "CHECK" : "CALL $%d", botToCall);

                        players[p].chips -= botToCall; players[p].currentBet += botToCall; state.pot += botToCall;
                        PlaySound(botToCall == 0 ? sndClick : sndChips);
                    } else if (action == 2) {
                        int botRaise = state.pot / 3; if (botRaise < 50) botRaise = 50;
                        int totalBotAdd = botToCall + botRaise;
                        if (totalBotAdd >= players[p].chips) {
                            totalBotAdd = players[p].chips; players[p].isAllIn = 1; sprintf(actionText[p], "ALL-IN");
                        } else sprintf(actionText[p], "RAISE $%d", botRaise);

                        players[p].chips -= totalBotAdd; players[p].currentBet += totalBotAdd; state.pot += totalBotAdd;
                        state.currentHighestBet = players[p].currentBet;

                        for(int i=0; i<4; i++) if (i != p) hasActed[i] = 0;
                        PlaySound(sndChips);
                    }
                    hasActed[p] = 1; actionJustTaken = true;
                }
            }
            else if (players[currentTurn].hasFolded || players[currentTurn].isAllIn) {
                hasActed[currentTurn] = 1; actionJustTaken = true;
            }

            if (actionJustTaken) {
                actionJustTaken = false;
                int activeInHand = 0, unsettled = 0;
                for (int i=0; i<4; i++) {
                    if (!players[i].hasFolded) {
                        activeInHand++;
                        if (!players[i].isAllIn) {
                            if (!hasActed[i] || players[i].currentBet < state.currentHighestBet) unsettled++;
                        }
                    }
                }

                if (activeInHand == 1) {
                    state.phase = PHASE_SHOWDOWN;
                    for(int i=0; i<4; i++) if(!players[i].hasFolded) winnerIndex = i;
                    players[winnerIndex].chips += state.pot;
                    if (winnerIndex == 0) PlaySound(sndWin);
                }
                else if (unsettled == 0 && activeInHand > 0) {
                    for(int i=0; i<4; i++) { players[i].currentBet = 0; hasActed[i] = 0; actionText[i][0] = '\0'; }
                    state.currentHighestBet = 0;

                    if (state.phase == PHASE_PREFLOP) { state.phase = PHASE_FLOP; for(int i=0; i<3; i++) communityCards[i] = DrawCard(&deck); }
                    else if (state.phase == PHASE_FLOP) { state.phase = PHASE_TURN; communityCards[3] = DrawCard(&deck); }
                    else if (state.phase == PHASE_TURN) { state.phase = PHASE_RIVER; communityCards[4] = DrawCard(&deck); }
                    else if (state.phase == PHASE_RIVER) {
                        state.phase = PHASE_SHOWDOWN;
                        HandScore bestScore = {0, {0}};
                        for (int p = 0; p < activeNumPlayers; p++) {
                            if (!players[p].hasFolded) {
                                HandScore currentScore = EvaluateHand(players[p].hand, communityCards);
                                if (winnerIndex == -1 || CompareHands(currentScore, bestScore) > 0) {
                                    bestScore = currentScore; winnerIndex = p; winningCategory = currentScore.category;
                                }
                            }
                        }
                        players[winnerIndex].chips += state.pot;
                        if (winnerIndex == 0) PlaySound(sndWin);
                    }
                    currentTurn = 1; actionTimer = GetTime();
                }
                else {
                    do { currentTurn = (currentTurn + 1) % 4; }
                    while (players[currentTurn].hasFolded || players[currentTurn].isAllIn);
                    actionTimer = GetTime();
                }
            }
        }
        else if (state.phase == PHASE_SHOWDOWN) {
            if (CheckCollisionPointRec(mousePoint, btnReset.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                PlaySound(sndClick); SaveChips(players[0].chips);

                if (players[0].chips <= 0) inMainMenu = true;
                else {
                    for(int i=1; i<activeNumPlayers; i++) if (players[i].chips <= 0) players[i].chips = 1000;

                    InitDeck(&deck); BogoShuffle(&deck, rand() % DECK_SIZE + 50);
                    DealStartingHands(&deck, players, activeNumPlayers);
                    for(int i=0; i<5; i++) communityCards[i].rank = 0;
                    for(int i=0; i<4; i++) { players[i].hasFolded = 0; players[i].isAllIn = 0; players[i].currentBet = 0; hasActed[i] = 0; actionText[i][0] = '\0'; }

                    if (players[1].chips >= 10) { players[1].chips -= 10; players[1].currentBet = 10; } else { players[1].currentBet = players[1].chips; players[1].chips = 0; players[1].isAllIn = 1; }
                    if (players[2].chips >= 20) { players[2].chips -= 20; players[2].currentBet = 20; } else { players[2].currentBet = players[2].chips; players[2].chips = 0; players[2].isAllIn = 1; }

                    state.pot = players[1].currentBet + players[2].currentBet;
                    state.currentHighestBet = 20;
                    state.phase = PHASE_PREFLOP; winnerIndex = -1;

                    currentTurn = 3; actionTimer = GetTime();
                }
            }
        }

        // ========================================================
        // GRAFICA MESEI
        // ========================================================
        BeginDrawing();
        ClearBackground((Color){20, 100, 40, 255});

        Rectangle tableSource = { 0.0f, 0.0f, (float)tableBg.width, (float)tableBg.height };
        Rectangle tableDest = { 0.0f, 0.0f, screenWidth, screenHeight };
        DrawTexturePro(tableBg, tableSource, tableDest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f)); // Ambient shading

        float cardScale = 0.45f;
        float scaledWidth = 167.5f * cardScale;

        // grafica CARTI COMUNE
        float pulse = sin(GetTime() * 6.0f) * 4.0f;
        int potFontSize = 30 + (int)pulse;
        char potText[50]; sprintf(potText, "POT: $%d", state.pot);
        DrawText(potText, screenWidth - 200, 50, potFontSize, GOLD);

        // grafica JUCĂTOR
        Vector2 myPos1 = { 500, screenHeight - 160 };
        Vector2 myPos2 = { 500+ scaledWidth + 10, screenHeight - 160 };

        char chipsText[50]; sprintf(chipsText, "Banii Tai: $%d", players[0].chips);
        DrawText(chipsText, myPos1.x, myPos1.y - 60, 25, GREEN);

        if (state.phase != PHASE_SHOWDOWN) {
            if (currentTurn == 0) DrawText("E randul tau!", myPos1.x, myPos1.y - 30, 20, WHITE);
            if (actionText[0][0] != '\0') DrawText(actionText[0], myPos1.x + (scaledWidth*2)/2 - MeasureText(actionText[0], 25)/2, myPos1.y + (239 * cardScale) + 10, 25, ORANGE);
        }

        // grafica BOTI
        Vector2 oppPositions[3] = {
            { 80, screenHeight/2 - 90 },                                 // Bot stânga
            { screenWidth/2 - scaledWidth - 5, 120 },                    // Bot sus
            { screenWidth - 80 - (scaledWidth*2), screenHeight/2 - 90 }  // Bot dreapta
        };

        if (state.phase == PHASE_SHOWDOWN && winnerIndex != -1) {
            float glowAlpha = (sin(GetTime() * 8.0f) + 1.0f) / 2.0f;
            Color glowColor = Fade(GOLD, glowAlpha * 0.7f);
            if (winnerIndex == 0) DrawRectangleRounded((Rectangle){myPos1.x - 10, myPos1.y - 10, (scaledWidth * 2) + 30, (239 * cardScale) + 20}, 0.2f, 10, glowColor);
            else DrawRectangleRounded((Rectangle){oppPositions[winnerIndex - 1].x - 10, oppPositions[winnerIndex - 1].y - 10, (scaledWidth * 2) + 30, (239 * cardScale) + 20}, 0.2f, 10, glowColor);
        }

        // carti JUCATOR
        if (!players[0].hasFolded) {
            DrawSpriteCard(players[0].hand[0], myPos1, cardScale, spriteSheet);
            DrawSpriteCard(players[0].hand[1], myPos2, cardScale, spriteSheet);
            if (players[0].isAllIn) DrawText("ALL-IN", myPos1.x + 30, myPos1.y + 40, 25, PURPLE);
        } else DrawText("AI DAT FOLD", myPos1.x + 10, myPos1.y + 40, 30, RED);

        // BOTI
        for (int p = 1; p < activeNumPlayers; p++) {
            Vector2 p1 = oppPositions[p-1];
            Vector2 p2 = { p1.x + 40, p1.y };

            char playerName[50];
            if (state.phase == PHASE_PREFLOP && p == 1) sprintf(playerName, "Jucatorul 2 (SB) - $%d", players[p].chips);
            else if (state.phase == PHASE_PREFLOP && p == 2) sprintf(playerName, "Jucatorul 3 (BB) - $%d", players[p].chips);
            else sprintf(playerName, "Jucatorul %d ($%d)", p + 1, players[p].chips);

            if (players[p].hasFolded) {
                DrawText("FOLD", p1.x + 30, p1.y + 50, 30, GRAY);
                DrawText(playerName, p1.x, p1.y - 25, 20, GRAY);
            } else {
                Color nameColor = players[p].isAllIn ? PURPLE : LIGHTGRAY;
                DrawText(playerName, p1.x, p1.y - 25, 20, nameColor);

                if (actionText[p][0] != '\0') DrawText(actionText[p], p1.x, p1.y + (239 * cardScale) + 10, 20, ORANGE);
                else if (currentTurn == p && state.phase != PHASE_SHOWDOWN) {
                    float pulseThink = (sin(GetTime() * 10.0f) + 1.0f) / 2.0f;
                    DrawText("Se gandeste...", p1.x, p1.y + (239 * cardScale) + 10, 20, Fade(YELLOW, pulseThink));
                    DrawRectangleRoundedLines((Rectangle){p1.x - 2, p1.y - 2, (scaledWidth) + 44, (239 * cardScale) + 4}, 0.2f, 2.0f, YELLOW);
                }

                if (state.phase != PHASE_SHOWDOWN) { DrawCardBack(p1, cardScale, spriteSheet); DrawCardBack(p2, cardScale, spriteSheet); }
                else { DrawSpriteCard(players[p].hand[0], p1, cardScale, spriteSheet); DrawSpriteCard(players[p].hand[1], p2, cardScale, spriteSheet); }
            }
        }

        // CARTI COMUNE CENTRATE
        float communityStartX = 430.0f; // Centrat vizual
        if (state.phase >= PHASE_FLOP) for (int i = 0; i < 3; i++) DrawSpriteCard(communityCards[i], (Vector2){ communityStartX + (i * (scaledWidth + 15)), screenHeight / 2.0f - 60.0f }, cardScale, spriteSheet);
        if (state.phase >= PHASE_TURN) DrawSpriteCard(communityCards[3], (Vector2){ communityStartX + (3 * (scaledWidth + 15)), screenHeight / 2.0f - 60.0f }, cardScale, spriteSheet);
        if (state.phase >= PHASE_RIVER) DrawSpriteCard(communityCards[4], (Vector2){ communityStartX + (4 * (scaledWidth + 15)), screenHeight / 2.0f - 60.0f }, cardScale, spriteSheet);

        // Meniu Butoane DOAR DACA E RANDUL JUCATORULUI
        if (state.phase != PHASE_SHOWDOWN && currentTurn == 0 && !players[0].hasFolded && !players[0].isAllIn) {
            if (!isTypingRaise) {
                DrawAndCheckButton(btnFold);
                DrawAndCheckButton(btnCall);
                if (!isForcedAllIn) DrawAndCheckButton(btnRaise);
                DrawAndCheckButton(btnAllIn);
            } else {
                DrawRectangleRounded((Rectangle){screenWidth - 350, screenHeight - 120, 320, 100}, 0.2f, 10, Fade(BLACK, 0.9f));
                DrawText("Tastati suma si apasati ENTER:", screenWidth - 330, screenHeight - 100, 15, LIGHTGRAY);
                char inputText[50]; sprintf(inputText, "$ %d", raiseAmount);
                DrawText(inputText, screenWidth - 190 - MeasureText(inputText, 40)/2, screenHeight - 70, 40, WHITE);
            }
        } else if (state.phase == PHASE_SHOWDOWN) {
            DrawAndCheckButton(btnReset);
            DrawRectangle(0, screenHeight/2 - 80, screenWidth, 150, Fade(BLACK, 0.8f));

            const char* handName = GetHandName(winningCategory);
            if (winnerIndex == 0) {
                DrawText("AI CASTIGAT!", screenWidth/2 - MeasureText("AI CASTIGAT!", 50)/2, screenHeight/2 - 60, 50, GREEN);
                char winText[100]; sprintf(winText, "Mana castigatoare: %s", handName);
                DrawText(winText, screenWidth/2 - MeasureText(winText, 30)/2, screenHeight/2, 30, LIGHTGRAY);
            } else {
                char loseText[50]; sprintf(loseText, "Jucatorul %d a castigat.", winnerIndex + 1);
                DrawText(loseText, screenWidth/2 - MeasureText(loseText, 40)/2, screenHeight/2 - 60, 40, RED);
                char winText[100]; sprintf(winText, "S-a castigat cu: %s", handName);
                DrawText(winText, screenWidth/2 - MeasureText(winText, 30)/2, screenHeight/2, 30, LIGHTGRAY);
            }
        }

        EndDrawing();
    }

    UnloadTexture(spriteSheet); UnloadTexture(bgMenu); UnloadTexture(tableBg);
    UnloadSound(sndClick); UnloadSound(sndChips); UnloadSound(sndFold); UnloadSound(sndWin);
    CloseAudioDevice(); CloseWindow();

    return 0;
}
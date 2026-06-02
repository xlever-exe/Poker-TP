#include "cards.h"

int CompareHands(HandScore a, HandScore b) {
    if (a.category > b.category) return 1;
    if (a.category < b.category) return -1;

    // Dacă categoriile sunt egale, comparăm valorile/kickerele
    for (int i = 0; i < 5; i++) {
        if (a.values[i] > b.values[i]) return 1;
        if (a.values[i] < b.values[i]) return -1;
    }
    return 0; // Remiză
}

// Funcție ajutătoare pentru a verifica dacă există 5 cărți consecutive
int GetStraightHigh(int rank_counts[15]) {
    // Verifică Straight normal (de la 5 până la Ace)
    for (int i = 10; i >= 2; i--) {
        if (rank_counts[i] && rank_counts[i+1] && rank_counts[i+2] && rank_counts[i+3] && rank_counts[i+4])
            return i + 4;
    }
    // Verifică Straight "A-2-3-4-5" (Wheel)
    if (rank_counts[14] && rank_counts[2] && rank_counts[3] && rank_counts[4] && rank_counts[5])
        return 5;
    return 0;
}

HandScore EvaluateHand(Card hole[2], Card community[5]) {
    HandScore score = {HIGH_CARD, {0}};
    Card cards[7];
    cards[0] = hole[0]; cards[1] = hole[1];
    for(int i = 0; i < 5; i++) cards[i+2] = community[i];

    int rank_counts[15] = {0};
    int suit_counts[4] = {0};
    int suit_rank_mask[4][15] = {0}; // Pentru Straight Flush

    for(int i = 0; i < 7; i++) {
        rank_counts[cards[i].rank]++;
        suit_counts[cards[i].suit]++;
        suit_rank_mask[cards[i].suit][cards[i].rank] = 1;
    }

    // 1. Verificare Straight Flush / Flush
    int flushSuit = -1;
    for(int s = 0; s < 4; s++) if(suit_counts[s] >= 5) flushSuit = s;

    if (flushSuit != -1) {
        int sfHigh = GetStraightHigh(suit_rank_mask[flushSuit]);
        if (sfHigh > 0) {
            score.category = STRAIGHT_FLUSH;
            score.values[0] = sfHigh;
            return score;
        }
    }

    // 2. Verificare Careu (Quads)
    for (int i = 14; i >= 2; i--) {
        if (rank_counts[i] == 4) {
            score.category = FOUR_OF_A_KIND;
            score.values[0] = i;
            for (int j = 14; j >= 2; j--) if (rank_counts[j] > 0 && j != i) { score.values[1] = j; break; }
            return score;
        }
    }

    // 3. Verificare Full House
    int trips = 0, pair1 = 0, pair2 = 0;
    for (int i = 14; i >= 2; i--) {
        if (rank_counts[i] == 3 && trips == 0) trips = i;
        else if (rank_counts[i] >= 2) {
            if (pair1 == 0) pair1 = i;
            else pair2 = i;
        }
    }
    if (trips > 0 && (pair1 > 0 || (rank_counts[trips] > 3))) { // Caz special dacă ai două seturi de 3
        score.category = FULL_HOUSE;
        score.values[0] = trips;
        score.values[1] = (pair1 > 0) ? pair1 : (trips > 3 ? trips : 0);
        return score;
    }

    // 4. Verificare Flush
    if (flushSuit != -1) {
        score.category = FLUSH;
        int v = 0;
        for (int i = 14; i >= 2 && v < 5; i--) if (suit_rank_mask[flushSuit][i]) score.values[v++] = i;
        return score;
    }

    // 5. Verificare Straight
    int st = GetStraightHigh(rank_counts);
    if (st > 0) {
        score.category = STRAIGHT;
        score.values[0] = st;
        return score;
    }

    // 6. Restul (Trips, Two Pair, One Pair, High Card)
    if (trips > 0) {
        score.category = THREE_OF_A_KIND;
        score.values[0] = trips;
        int v = 1;
        for (int i = 14; i >= 2 && v < 3; i--) if (rank_counts[i] > 0 && i != trips) score.values[v++] = i;
    } else if (pair1 > 0 && pair2 > 0) {
        score.category = TWO_PAIR;
        score.values[0] = pair1; score.values[1] = pair2;
        for (int i = 14; i >= 2; i--) if (rank_counts[i] > 0 && i != pair1 && i != pair2) { score.values[2] = i; break; }
    } else if (pair1 > 0) {
        score.category = ONE_PAIR;
        score.values[0] = pair1;
        int v = 1;
        for (int i = 14; i >= 2 && v < 4; i--) if (rank_counts[i] > 0 && i != pair1) score.values[v++] = i;
    } else {
        score.category = HIGH_CARD;
        int v = 0;
        for (int i = 14; i >= 2 && v < 5; i--) if (rank_counts[i] > 0) score.values[v++] = i;
    }

    return score;
}
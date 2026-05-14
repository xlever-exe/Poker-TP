#include "cards.h"

int CompareHands(HandScore a, HandScore b) {
    if (a.category > b.category) return 1;
    if (a.category < b.category) return -1;

   
    for (int i = 0; i < 5; i++) {
        if (a.values[i] > b.values[i]) return 1;
        if (a.values[i] < b.values[i]) return -1;
    }
    return 0; // Split pot
}

HandScore EvaluateHand(Card hole[2], Card community[5]) {
    HandScore score = {HIGH_CARD, {0}};

   
    Card cards[7];
    cards[0] = hole[0]; cards[1] = hole[1];
    for(int i = 0; i < 5; i++) cards[i+2] = community[i];

 
    int rank_counts[15] = {0}; 
    int suit_counts[4] = {0};

    for(int i = 0; i < 7; i++) {
        rank_counts[cards[i].rank]++;
        suit_counts[cards[i].suit]++;
    }

   
    int quads = 0, trips = 0;
    int pairs[2] = {0, 0};

    for(int i = 14; i >= 2; i--) {
        if (rank_counts[i] == 4) quads = i;
        else if (rank_counts[i] == 3) {
            if (trips == 0) trips = i;
            else pairs[0] = i; 
        }
        else if (rank_counts[i] == 2) {
            if (pairs[0] == 0) pairs[0] = i;
            else if (pairs[1] == 0) pairs[1] = i;
        }
    }

   


    if (quads > 0) {
        score.category = FOUR_OF_A_KIND;
        score.values[0] = quads;
        // Find the single highest kicker
        for(int i = 14; i >= 2; i--) {
            if (rank_counts[i] > 0 && i != quads) {
                score.values[1] = i;
                break;
            }
        }
    }
    else if (trips > 0 && pairs[0] > 0) {
        score.category = FULL_HOUSE;
        score.values[0] = trips;    
        score.values[1] = pairs[0]; 
    }
    else if (trips > 0) {
        score.category = THREE_OF_A_KIND;
        score.values[0] = trips;
        int v_idx = 1;
        for(int i = 14; i >= 2 && v_idx < 3; i--) {
            if (rank_counts[i] > 0 && i != trips) {
                score.values[v_idx++] = i; 
            }
        }
    }
    else if (pairs[0] > 0 && pairs[1] > 0) {
        score.category = TWO_PAIR;
        score.values[0] = pairs[0]; 
        score.values[1] = pairs[1]; 
        for(int i = 14; i >= 2; i--) {
            if (rank_counts[i] > 0 && i != pairs[0] && i != pairs[1]) {
                score.values[2] = i; 
                break;
            }
        }
    }
    else if (pairs[0] > 0) {
        score.category = ONE_PAIR;
        score.values[0] = pairs[0];
        int v_idx = 1;
        for(int i = 14; i >= 2 && v_idx < 4; i--) {
            if (rank_counts[i] > 0 && i != pairs[0]) {
                score.values[v_idx++] = i; 
            }
        }
    }
    else {
        score.category = HIGH_CARD;
        int v_idx = 0;
        for(int i = 14; i >= 2 && v_idx < 5; i--) {
            if (rank_counts[i] > 0) {
                score.values[v_idx++] = i; 
            }
        }
    }

    return score;
}

#pragma once
#include <stdbool.h>

typedef enum {
    AI_EASY = 0,    // random
    AI_MEDIUM,      // minimax depth 2–3
    AI_HARD         // minimax depth 4–5
} AIDifficulty;

extern AIDifficulty ai_difficulty;

// Set AI difficulty
void set_ai_difficulty(AIDifficulty diff);

// AI move (returns true if a move was made)
bool ai_move(int board[8][8], int color, int search_depth, AIDifficulty diff);

// Minimax interface
float evaluate_board(int board[8][8], int color);
float minimax(int board[8][8], int depth, float alpha, float beta, int maximizingPlayer, int color, int *out_fr, int *out_fc, int *out_tr, int *out_tc);
#pragma once
#include "ui.h"
#include <stdbool.h>

// Game state struct to save/load (expandable)
typedef struct {
    int board[8][8];
    int current_turn;
    char last_move[32];
    char message[64];
    float eval_score;
    int show_eval;
} SaveGameState;

bool save_game(const SaveGameState *state, const char *filename);
bool load_game(SaveGameState *state, const char *filename);
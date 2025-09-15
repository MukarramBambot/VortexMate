#pragma once

// Evaluate board from the perspective of 'color': + means good for White, - for Black
float evaluate_board(int board[8][8], int color);

// Returns true if AI moved, false if no valid move (checkmate/stalemate)
bool ai_move(int board[8][8], int color, int search_depth);

// Minimax with alpha-beta pruning
float minimax(int board[8][8], int depth, float alpha, float beta, int maximizingPlayer, int color, int *out_fr, int *out_fc, int *out_tr, int *out_tc);
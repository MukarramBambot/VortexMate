#pragma once
#include <stdbool.h>

// Turn system
typedef enum { WHITE_TURN = 1, BLACK_TURN = -1 } Turn;

extern Turn current_turn;

// Move logic
bool is_valid_move(int board[8][8], int fr, int fc, int tr, int tc);
void apply_move(int board[8][8], int fr, int fc, int tr, int tc);

// Special move helpers
bool can_castle(int board[8][8], int fr, int fc, int tr, int tc);
bool can_en_passant(int board[8][8], int fr, int fc, int tr, int tc);
void promote_pawn(int board[8][8], int row, int col);

// Move history for special moves
void reset_move_state();
void update_move_state(int fr, int fc, int tr, int tc, int movedPiece);

bool is_opponent_piece(int board[8][8], int fr, int fc, int tr, int tc);
bool is_same_color(int board[8][8], int fr, int fc, int tr, int tc);
bool is_path_clear(int board[8][8], int fr, int fc, int tr, int tc);
bool is_empty(int board[8][8], int row, int col);

// Check/checkmate/stalemate
bool is_in_check(int board[8][8], int color);    // color: 1 (white) or -1 (black)
<<<<<<< HEAD
bool has_valid_moves(int board[8][8], int color);
=======
bool has_valid_moves(int board[8][8], int color);

// New helper: validate moves without considering check
bool is_valid_move_no_check(int board[8][8], int fr, int fc, int tr, int tc);
>>>>>>> a3d94f5 (solving errors)

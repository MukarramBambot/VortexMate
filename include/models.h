#pragma once
#include "raylib.h"
#include <stdbool.h>

// Piece macros
#define EMPTY     0
#define W_PAWN    1
#define W_ROOK    2
#define W_KNIGHT  3
#define W_BISHOP  4
#define W_QUEEN   5
#define W_KING    6
#define B_PAWN   -1
#define B_ROOK   -2
#define B_KNIGHT -3
#define B_BISHOP -4
#define B_QUEEN  -5
#define B_KING   -6

// Color constants
#define WHITE 1
#define BLACK -1

extern Color WHITE_MAIN, WHITE_ACCENT, BLACK_MAIN, BLACK_ACCENT, BASE_RING;

void draw_piece3d(int piece, int row, int col, Camera camera, float anim_scale, float anim_alpha);
void init_board(int board[8][8]);
Vector3 board_to_world(int row, int col);

// Mouse picking: returns true if a board tile is clicked, outputs row/col
bool pick_tile(Camera camera, Vector2 mouse, int *out_row, int *out_col);

// Utility: string name for a piece (e.g., "W_PAWN")
const char* piece_name(int piece);
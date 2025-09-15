#include "chess_ai.h"
#include "chess_logic.h"
#include "models.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <float.h>

#define MAX_MOVES 256

// Piece values
static const float piece_value[7] = {0, 1.0f, 5.0f, 3.0f, 3.0f, 9.0f, 100.0f}; // EMPTY=0, PAWN=1, ROOK=5, KNIGHT=3, BISHOP=3, QUEEN=9, KING=100

// Utility: material evaluation (simple version)
float evaluate_board(int board[8][8], int color) {
    float score = 0.0f;
    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
        int piece = board[r][c];
        if (piece == EMPTY) continue;
        int abs_piece = (piece > 0) ? piece : -piece;
        float val = piece_value[abs_piece];
        if (piece > 0) score += val;
        else score -= val;
    }
    // Positive means White is winning, negative means Black is winning.
    return score * color;
}

// Move struct
typedef struct {
    int fr, fc, tr, tc;
} Move;

// Generates all valid moves for 'color', returns count, fills moves array.
static int generate_all_valid_moves(int board[8][8], int color, Move moves[MAX_MOVES]) {
    int count = 0;
    for (int fr = 0; fr < 8; fr++) for (int fc = 0; fc < 8; fc++) {
        int piece = board[fr][fc];
        if (piece == EMPTY || ((piece > 0) != (color > 0))) continue;
        for (int tr = 0; tr < 8; tr++) for (int tc = 0; tc < 8; tc++) {
            if (is_valid_move(board, fr, fc, tr, tc)) {
                if (count < MAX_MOVES)
                    moves[count++] = (Move){fr, fc, tr, tc};
            }
        }
    }
    return count;
}

// Minimax with alpha-beta pruning
float minimax(int board[8][8], int depth, float alpha, float beta, int maximizingPlayer, int color, int *out_fr, int *out_fc, int *out_tr, int *out_tc) {
    int current_color = maximizingPlayer ? color : -color;

    // Detect endgame
    if (depth == 0 || !has_valid_moves(board, current_color)) {
        float eval = evaluate_board(board, color);
        int in_check = is_in_check(board, current_color);
        if (!has_valid_moves(board, current_color)) {
            if (in_check)
                eval = (current_color == color) ? -999.0f : 999.0f; // If current color is checkmated, bad for them
            else
                eval = 0.0f; // stalemate
        }
        return eval;
    }

    Move moves[MAX_MOVES];
    int move_count = generate_all_valid_moves(board, current_color, moves);

    // If no moves, return evaluation
    if (move_count == 0)
        return evaluate_board(board, color);

    int best_indices[MAX_MOVES];
    int best_count = 0;
    float best_eval = maximizingPlayer ? -FLT_MAX : FLT_MAX;

    for (int i = 0; i < move_count; i++) {
        int tmp_board[8][8];
        memcpy(tmp_board, board, sizeof(tmp_board));
        int fr = moves[i].fr, fc = moves[i].fc, tr = moves[i].tr, tc = moves[i].tc;
        // Actually make the move on tmp_board
        int moved_piece = tmp_board[fr][fc];
        int captured = tmp_board[tr][tc];
        tmp_board[tr][tc] = moved_piece;
        tmp_board[fr][fc] = EMPTY;
        // Pawn promotion
        if ((moved_piece == W_PAWN && tr == 0) || (moved_piece == B_PAWN && tr == 7))
            tmp_board[tr][tc] = (moved_piece > 0) ? W_QUEEN : B_QUEEN;

        float eval = minimax(tmp_board, depth-1, alpha, beta, !maximizingPlayer, color, NULL, NULL, NULL, NULL);

        if (maximizingPlayer) {
            if (eval > best_eval) {
                best_eval = eval;
                best_indices[0] = i; best_count = 1;
            } else if (eval == best_eval) {
                best_indices[best_count++] = i;
            }
            if (eval > alpha) alpha = eval;
        } else {
            if (eval < best_eval) {
                best_eval = eval;
                best_indices[0] = i; best_count = 1;
            } else if (eval == best_eval) {
                best_indices[best_count++] = i;
            }
            if (eval < beta) beta = eval;
        }
        if (beta <= alpha) break; // Alpha-beta
    }

    // If at root node, output best move
    if (out_fr && best_count > 0) {
        // Randomly choose among best moves if there are ties
        int idx = best_indices[rand() % best_count];
        *out_fr = moves[idx].fr;
        *out_fc = moves[idx].fc;
        *out_tr = moves[idx].tr;
        *out_tc = moves[idx].tc;
    }

    return best_eval;
}

// AI move for black (color = -1), for now
bool ai_move(int board[8][8], int color, int search_depth) {
    Move moves[MAX_MOVES];
    int move_count = generate_all_valid_moves(board, color, moves);

    if (move_count == 0) return false;

    int best_fr = -1, best_fc = -1, best_tr = -1, best_tc = -1;

    srand((unsigned int)time(NULL) ^ (unsigned int)rand());

    float eval = minimax(board, search_depth, -FLT_MAX, FLT_MAX, 1, color, &best_fr, &best_fc, &best_tr, &best_tc);

    if (best_fr == -1) return false;

    // Logging
    int moved = board[best_fr][best_fc];
    char col_to_file(int col) { return 'a' + col; }
    char row_to_rank(int row) { return '1' + (7 - row); }
    printf("AI plays: %s %c%d → %c%d (eval = %+0.2f)\n",
        piece_name(moved),
        col_to_file(best_fc), row_to_rank(best_fr),
        col_to_file(best_tc), row_to_rank(best_tr),
        eval);

    apply_move(board, best_fr, best_fc, best_tr, best_tc);
    return true;
}
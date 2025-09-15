#include "ai.h"
#include "chess_logic.h"
#include "models.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <float.h>

AIDifficulty ai_difficulty = AI_MEDIUM;

void set_ai_difficulty(AIDifficulty diff) {
    ai_difficulty = diff;
}

// Piece values for evaluation
static const float piece_value[7] = {0, 1.0f, 5.0f, 3.0f, 3.0f, 9.0f, 100.0f};

// Move struct
typedef struct {
    int fr, fc, tr, tc;
} Move;

#define MAX_MOVES 256

// Generate all valid moves for a color
static int generate_all_valid_moves(int board[8][8], int color, Move moves[MAX_MOVES]) {
    int count = 0;
    for (int fr = 0; fr < 8; fr++) {
        for (int fc = 0; fc < 8; fc++) {
            int piece = board[fr][fc];
            if (piece == EMPTY || ((piece > 0) != (color > 0))) continue;
            for (int tr = 0; tr < 8; tr++) {
                for (int tc = 0; tc < 8; tc++) {
                    if (is_valid_move(board, fr, fc, tr, tc)) {
                        if (count < MAX_MOVES) {
                            moves[count++] = (Move){fr, fc, tr, tc};
                        }
                    }
                }
            }
        }
    }
    return count;
}

// Simple board evaluation
float evaluate_board(int board[8][8], int color) {
    float score = 0.0f;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int piece = board[r][c];
            if (piece == EMPTY) continue;
            int abs_piece = (piece > 0) ? piece : -piece;
            float val = piece_value[abs_piece];
            if (piece > 0) score += val;
            else score -= val;
        }
    }
    return score * color;
}

// Minimax with alpha-beta pruning
float minimax(int board[8][8], int depth, float alpha, float beta, int maximizingPlayer, int color, int *out_fr, int *out_fc, int *out_tr, int *out_tc) {
    int current_color = maximizingPlayer ? color : -color;

    if (depth == 0 || !has_valid_moves(board, current_color)) {
        float eval = evaluate_board(board, color);
        bool in_check = is_in_check(board, current_color);
        if (!has_valid_moves(board, current_color)) {
            if (in_check)
                eval = (current_color == color) ? -999.0f : 999.0f;
            else
                eval = 0.0f; // stalemate
        }
        return eval;
    }

    Move moves[MAX_MOVES];
    int move_count = generate_all_valid_moves(board, current_color, moves);

    if (move_count == 0)
        return evaluate_board(board, color);

    int best_indices[MAX_MOVES];
    int best_count = 0;
    float best_eval = maximizingPlayer ? -FLT_MAX : FLT_MAX;

    for (int i = 0; i < move_count; i++) {
        int tmp_board[8][8];
        memcpy(tmp_board, board, sizeof(tmp_board));
        int fr = moves[i].fr, fc = moves[i].fc, tr = moves[i].tr, tc = moves[i].tc;
        
        // Make the move
        int moved_piece = tmp_board[fr][fc];
        tmp_board[tr][tc] = moved_piece;
        tmp_board[fr][fc] = EMPTY;
        
        // Pawn promotion
        if ((moved_piece == W_PAWN && tr == 0) || (moved_piece == B_PAWN && tr == 7))
            tmp_board[tr][tc] = (moved_piece > 0) ? W_QUEEN : B_QUEEN;

        float eval = minimax(tmp_board, depth-1, alpha, beta, !maximizingPlayer, color, NULL, NULL, NULL, NULL);

        if (maximizingPlayer) {
            if (eval > best_eval) {
                best_eval = eval;
                best_indices[0] = i; 
                best_count = 1;
            } else if (eval == best_eval) {
                best_indices[best_count++] = i;
            }
            if (eval > alpha) alpha = eval;
        } else {
            if (eval < best_eval) {
                best_eval = eval;
                best_indices[0] = i; 
                best_count = 1;
            } else if (eval == best_eval) {
                best_indices[best_count++] = i;
            }
            if (eval < beta) beta = eval;
        }
        if (beta <= alpha) break;
    }

    // Output best move at root
    if (out_fr && best_count > 0) {
        int idx = best_indices[rand() % best_count];
        *out_fr = moves[idx].fr;
        *out_fc = moves[idx].fc;
        *out_tr = moves[idx].tr;
        *out_tc = moves[idx].tc;
    }

    return best_eval;
}

// AI move implementation
bool ai_move(int board[8][8], int color, int search_depth, AIDifficulty diff) {
    Move moves[MAX_MOVES];
    int count = generate_all_valid_moves(board, color, moves);
    if (count == 0) return false;
    
    int fr = -1, fc = -1, tr = -1, tc = -1;
    
    if (diff == AI_EASY) {
        srand((unsigned int)time(NULL) ^ rand());
        int idx = rand() % count;
        fr = moves[idx].fr; 
        fc = moves[idx].fc; 
        tr = moves[idx].tr; 
        tc = moves[idx].tc;
    } else {
        int depth = (diff == AI_MEDIUM) ? (search_depth < 2 ? 2 : search_depth) : (search_depth < 4 ? 4 : search_depth);
        minimax(board, depth, -FLT_MAX, FLT_MAX, 1, color, &fr, &fc, &tr, &tc);
        if (fr == -1) { // fallback if minimax fails
            int idx = rand() % count;
            fr = moves[idx].fr; 
            fc = moves[idx].fc; 
            tr = moves[idx].tr; 
            tc = moves[idx].tc;
        }
    }
    
    if (fr == -1) return false;
    apply_move(board, fr, fc, tr, tc);
    return true;
}
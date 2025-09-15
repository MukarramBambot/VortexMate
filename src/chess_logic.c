#include "chess_logic.h"
#include "models.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// === Turn State ===
Turn current_turn = WHITE_TURN;

// Special move state as before ...
static bool white_king_moved = false;
static bool black_king_moved = false;
static bool white_rook_moved[2] = {false, false};
static bool black_rook_moved[2] = {false, false};
static int last_pawn_doublemove_row = -1, last_pawn_doublemove_col = -1;
static int last_pawn_doublemove_turn = 0; // 0: none, >0: turn count

void reset_move_state() {
    white_king_moved = false;
    black_king_moved = false;
    white_rook_moved[0] = white_rook_moved[1] = false;
    black_rook_moved[0] = black_rook_moved[1] = false;
    last_pawn_doublemove_row = -1; last_pawn_doublemove_col = -1;
    last_pawn_doublemove_turn = 0;
}
void update_move_state(int fr, int fc, int tr, int tc, int movedPiece) {
    if (movedPiece == W_KING) white_king_moved = true;
    if (movedPiece == B_KING) black_king_moved = true;
    if (movedPiece == W_ROOK) {
        if (fr == 7 && fc == 0) white_rook_moved[0] = true;
        if (fr == 7 && fc == 7) white_rook_moved[1] = true;
    }
    if (movedPiece == B_ROOK) {
        if (fr == 0 && fc == 0) black_rook_moved[0] = true;
        if (fr == 0 && fc == 7) black_rook_moved[1] = true;
    }
    if (abs(movedPiece) == W_PAWN && abs(tr - fr) == 2) {
        last_pawn_doublemove_row = tr;
        last_pawn_doublemove_col = tc;
        last_pawn_doublemove_turn = 1;
    } else {
        last_pawn_doublemove_row = -1;
        last_pawn_doublemove_col = -1;
        last_pawn_doublemove_turn = 0;
    }
}

// --- Utility ---
bool is_empty(int board[8][8], int row, int col) {
    return board[row][col] == EMPTY;
}
bool is_opponent_piece(int board[8][8], int fr, int fc, int tr, int tc) {
    int src = board[fr][fc], dest = board[tr][tc];
    if (dest == EMPTY) return false;
    return (src * dest) < 0;
}
bool is_same_color(int board[8][8], int fr, int fc, int tr, int tc) {
    int src = board[fr][fc], dest = board[tr][tc];
    if (src == EMPTY || dest == EMPTY) return false;
    return (src * dest) > 0;
}
bool is_path_clear(int board[8][8], int fr, int fc, int tr, int tc) {
    int dr = tr - fr, dc = tc - fc;
    int step_r = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
    int step_c = (dc == 0) ? 0 : (dc > 0 ? 1 : -1);
    int steps = (abs(dr) > abs(dc)) ? abs(dr) : abs(dc);
    if (!((dr == 0) || (dc == 0) || (abs(dr) == abs(dc)))) return false;
    int r = fr + step_r, c = fc + step_c;
    for (int i = 1; i < steps; i++, r += step_r, c += step_c) {
        if (!is_empty(board, r, c)) return false;
    }
    return true;
}

// --- Check Detection ---
static void find_king(int board[8][8], int color, int *krow, int *kcol) {
    int king = (color == 1) ? W_KING : B_KING;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (board[r][c] == king) {
                *krow = r; *kcol = c; return;
            }
    *krow = -1; *kcol = -1;
}

// Returns true if the king of `color` is under attack by any opponent piece.
bool is_in_check(int board[8][8], int color) {
    int krow, kcol;
    find_king(board, color, &krow, &kcol);
    if (krow == -1) return false; // Defensive fallback

    // For each opponent piece, check if it can move to king's square (ignoring king safety)
    for (int r=0; r<8; r++) for (int c=0; c<8; c++) {
        int piece = board[r][c];
        if (piece == EMPTY || ((piece > 0) == (color > 0))) continue;
        // Use special: is_valid_move_attack for pawns (because they only attack diagonally)
        int abs_piece = abs(piece);
        if (abs_piece == W_PAWN) {
            int dir = (piece > 0) ? -1 : 1;
            if (krow == r + dir && (kcol == c - 1 || kcol == c + 1))
                return true;
        } else {
            // For others, use is_valid_move ignoring king safety (no recursion!)
            if (is_valid_move_no_check(board, r, c, krow, kcol)) return true;
        }
    }
    return false;
}

// Valid move for rules, but does NOT check for leaving king in check (to avoid infinite recursion)
bool is_valid_move_no_check(int board[8][8], int fr, int fc, int tr, int tc);

// Returns true if the player of color has any valid legal move (not leaving king in check)
bool has_valid_moves(int board[8][8], int color) {
    for (int fr = 0; fr < 8; fr++) for (int fc = 0; fc < 8; fc++) {
        int piece = board[fr][fc];
        if (piece == EMPTY || ((piece > 0) != (color > 0))) continue;
        for (int tr = 0; tr < 8; tr++) for (int tc = 0; tc < 8; tc++) {
            // For castling, only try king's original square and two squares to the side
            if (abs(piece) == W_KING && fr == tr && abs(tc - fc) == 2 && fc == 4) {
                if (is_valid_move(board, fr, fc, tr, tc))
                    return true;
            } else if (is_valid_move(board, fr, fc, tr, tc)) {
                return true;
            }
        }
    }
    return false;
}

// --- Special move helpers (as before, unchanged) ---
bool can_castle(int board[8][8], int fr, int fc, int tr, int tc) {
    int piece = board[fr][fc];
    if (!(abs(piece) == W_KING && fr == tr)) return false;
    int color = (piece > 0) ? 1 : -1;
    int home_row = (color == 1) ? 7 : 0;
    if (fr != home_row || fc != 4) return false;
    if (abs(tc - fc) != 2) return false;
    int kingside = (tc - fc) > 0;
    int rook_col = kingside ? 7 : 0;
    int rook_piece = board[fr][rook_col];
    if (color == 1) {
        if (white_king_moved) return false;
        if (white_rook_moved[kingside ? 1 : 0]) return false;
    } else {
        if (black_king_moved) return false;
        if (black_rook_moved[kingside ? 1 : 0]) return false;
    }
    if ((color == 1 && rook_piece != W_ROOK) || (color == -1 && rook_piece != B_ROOK)) return false;
    // Path between king and rook must be clear
    int c1 = kingside ? fc+1 : rook_col+1;
    int c2 = kingside ? rook_col-1 : fc-1;
    for (int c = c1; c <= c2; c++) {
        if (!is_empty(board, fr, c)) return false;
    }
    // New: King cannot castle through, into, or out of check
    int kstep = (kingside ? 1 : -1);
    for (int i = 0; i <= 2; i++) {
        int tcol = fc + i*kstep;
        // Simulate king at each square
        int temp_board[8][8];
        memcpy(temp_board, board, sizeof(temp_board));
        temp_board[fr][fc] = EMPTY;
        temp_board[fr][tcol] = piece;
        if (is_in_check(temp_board, color)) return false;
    }
    return true;
}

bool can_en_passant(int board[8][8], int fr, int fc, int tr, int tc) {
    int piece = board[fr][fc];
    if (abs(piece) != W_PAWN) return false;
    int dir = (piece > 0) ? -1 : 1;
    if (abs(tc - fc) != 1 || tr - fr != dir) return false;
    if (tr == last_pawn_doublemove_row && tc == last_pawn_doublemove_col &&
        last_pawn_doublemove_turn == 1 &&
        abs(board[last_pawn_doublemove_row - dir][tc]) == W_PAWN &&
        (board[last_pawn_doublemove_row - dir][tc] * piece) < 0)
    {
        return true;
    }
    return false;
}

void promote_pawn(int board[8][8], int row, int col) {
    int piece = board[row][col];
    if (piece == W_PAWN && row == 0) {
        board[row][col] = W_QUEEN;
        printf("Pawn promoted to Queen at (%d,%d)\n", row, col);
    }
    if (piece == B_PAWN && row == 7) {
        board[row][col] = B_QUEEN;
        printf("Pawn promoted to Queen at (%d,%d)\n", row, col);
    }
}

// --- Move Validation with Check Detection ---
bool is_valid_move_no_check(int board[8][8], int fr, int fc, int tr, int tc) {
    int piece = board[fr][fc];
    if (piece == EMPTY) return false;
    if (fr == tr && fc == tc) return false;
    // No color/turn checks in this context (used for attack check)
    if (is_same_color(board, fr, fc, tr, tc)) return false;
    int dr = tr - fr, dc = tc - fc;
    int abs_dr = abs(dr), abs_dc = abs(dc);
    int dir = (piece > 0) ? -1 : 1;
    // Special moves not used for attack
    switch (abs(piece)) {
        case W_PAWN:
            // Only diagonal capture
            if (abs_dc == 1 && dr == dir && is_opponent_piece(board, fr, fc, tr, tc)) return true;
            return false;
        case W_ROOK:
            if ((dr == 0 || dc == 0) && is_path_clear(board, fr, fc, tr, tc)) return true;
            return false;
        case W_BISHOP:
            if (abs_dr == abs_dc && is_path_clear(board, fr, fc, tr, tc)) return true;
            return false;
        case W_QUEEN:
            if (((dr == 0 || dc == 0) || (abs_dr == abs_dc)) && is_path_clear(board, fr, fc, tr, tc)) return true;
            return false;
        case W_KNIGHT:
            if ((abs_dr == 2 && abs_dc == 1) || (abs_dr == 1 && abs_dc == 2)) return true;
            return false;
        case W_KING:
            if (abs_dr <= 1 && abs_dc <= 1) return true;
            return false;
        default:
            return false;
    }
}

// This is now the "full rules" move validator: move must not leave own king in check!
bool is_valid_move(int board[8][8], int fr, int fc, int tr, int tc) {
    if (fr < 0 || fr > 7 || fc < 0 || fc > 7 || tr < 0 || tr > 7 || tc < 0 || tc > 7)
        return false;
    int piece = board[fr][fc];
    if (piece == EMPTY) return false;
    if (fr == tr && fc == tc) return false;
    // Only allow correct color to move
    if ((current_turn == WHITE_TURN && piece < 0) ||
        (current_turn == BLACK_TURN && piece > 0))
        return false;
    // No capturing same color
    if (is_same_color(board, fr, fc, tr, tc)) return false;

    int dr = tr - fr, dc = tc - fc;
    int abs_dr = abs(dr), abs_dc = abs(dc);
    int dir = (piece > 0) ? -1 : 1;

    // --- Special moves first ---
    if (abs(piece) == W_KING && abs(tc - fc) == 2 && fr == tr) {
        if (!can_castle(board, fr, fc, tr, tc)) return false;
        // Ensure king not in check at any square passed
        return true;
    }
    if (abs(piece) == W_PAWN && can_en_passant(board, fr, fc, tr, tc)) {
        // Simulate the capture and make sure king is not in check after
        int temp[8][8]; memcpy(temp, board, sizeof(temp));
        int dir = (piece > 0) ? 1 : -1;
        temp[tr][tc] = piece;
        temp[fr][fc] = EMPTY;
        temp[tr+dir][tc] = EMPTY;
        int color = (piece > 0) ? 1 : -1;
        if (is_in_check(temp, color)) return false;
        return true;
    }

    // --- Normal moves ---
    bool move_ok = false;
    switch (abs(piece)) {
        case W_PAWN: {
            if (dc == 0 && is_empty(board, tr, tc)) {
                if (dr == dir) move_ok = true;
                if (((piece > 0 && fr == 6) || (piece < 0 && fr == 1)) && dr == 2*dir) {
                    int midr = fr + dir;
                    if (is_empty(board, midr, fc)) move_ok = true;
                }
            }
            if (abs_dc == 1 && dr == dir && is_opponent_piece(board, fr, fc, tr, tc)) move_ok = true;
            break;
        }
        case W_ROOK:
            if ((dr == 0 || dc == 0) && is_path_clear(board, fr, fc, tr, tc)) move_ok = true;
            break;
        case W_BISHOP:
            if (abs_dr == abs_dc && is_path_clear(board, fr, fc, tr, tc)) move_ok = true;
            break;
        case W_QUEEN:
            if (((dr == 0 || dc == 0) || (abs_dr == abs_dc)) && is_path_clear(board, fr, fc, tr, tc)) move_ok = true;
            break;
        case W_KNIGHT:
            if ((abs_dr == 2 && abs_dc == 1) || (abs_dr == 1 && abs_dc == 2)) move_ok = true;
            break;
        case W_KING:
            if (abs_dr <= 1 && abs_dc <= 1) move_ok = true;
            break;
        default:
            move_ok = false;
    }
    if (!move_ok) return false;

    // --- Simulate the move and check king safety ---
    int temp[8][8]; memcpy(temp, board, sizeof(temp));
    temp[tr][tc] = piece;
    temp[fr][fc] = EMPTY;
    if (abs(piece) == W_PAWN && ((piece > 0 && tr == 0) || (piece < 0 && tr == 7))) {
        // Promote for check check
        temp[tr][tc] = (piece > 0) ? W_QUEEN : B_QUEEN;
    }
    int color = (piece > 0) ? 1 : -1;
    if (is_in_check(temp, color)) return false;
    return true;
}

// --- Move Application (with check/checkmate/stalemate logging) ---
void apply_move(int board[8][8], int fr, int fc, int tr, int tc) {
    int piece = board[fr][fc];
    if (!is_valid_move(board, fr, fc, tr, tc)) {
        printf("Invalid move for piece: %s from (%d,%d) to (%d,%d)\n",
               piece_name(piece), fr, fc, tr, tc);
        return;
    }

    // --- Special moves ---
    if (abs(piece) == W_KING && abs(tc - fc) == 2 && fr == tr) {
        int kingside = (tc - fc) > 0;
        int rook_from = kingside ? 7 : 0, rook_to = kingside ? tc-1 : tc+1;
        board[tr][tc] = piece;
        board[fr][fc] = EMPTY;
        board[fr][rook_to] = board[fr][rook_from];
        board[fr][rook_from] = EMPTY;
        update_move_state(fr, fc, tr, tc, piece);
        update_move_state(fr, rook_from, fr, rook_to, board[fr][rook_to]);
        printf("Castling performed (%s King-side)\n",
               (kingside ? (piece > 0 ? "White" : "Black") : (piece > 0 ? "White Queen-side" : "Black Queen-side")));
        // Next turn
        current_turn = (current_turn == WHITE_TURN) ? BLACK_TURN : WHITE_TURN;
        goto end_of_move_checks;
    }
    // En passant
    if (abs(piece) == W_PAWN && can_en_passant(board, fr, fc, tr, tc)) {
        board[tr][tc] = piece;
        board[fr][fc] = EMPTY;
        int dir = (piece > 0) ? 1 : -1;
        int cap_row = tr + dir, cap_col = tc;
        printf("En Passant capture at (%d,%d)\n", cap_row, cap_col);
        board[cap_row][cap_col] = EMPTY;
        update_move_state(fr, fc, tr, tc, piece);
        current_turn = (current_turn == WHITE_TURN) ? BLACK_TURN : WHITE_TURN;
        goto end_of_move_checks;
    }
    // --- Normal move ---
    int captured = board[tr][tc];
    board[tr][tc] = piece;
    board[fr][fc] = EMPTY;
    update_move_state(fr, fc, tr, tc, piece);

    // Promotion
    if (abs(piece) == W_PAWN && ((piece > 0 && tr == 0) || (piece < 0 && tr == 7))) {
        promote_pawn(board, tr, tc);
    }

    printf("Moved %s from (%d,%d) to (%d,%d)%s\n",
        piece_name(piece), fr, fc, tr, tc,
        (captured != EMPTY ? " (capture)" : ""));
    current_turn = (current_turn == WHITE_TURN) ? BLACK_TURN : WHITE_TURN;

end_of_move_checks:
    // After move: check/checkmate/stalemate detection
    int next_color = (current_turn == WHITE_TURN) ? 1 : -1;
    if (is_in_check(board, next_color)) {
        printf("Check!\n");
        if (!has_valid_moves(board, next_color)) {
            printf("Checkmate!\n");
        }
    } else {
        if (!has_valid_moves(board, next_color)) {
            printf("Stalemate!\n");
        }
    }
}
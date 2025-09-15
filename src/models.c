#include "models.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "raymath.h"

// Color palettes
Color WHITE_MAIN  = {230, 245, 255, 255}; // Off-white
Color WHITE_ACCENT= {0, 255, 230, 255};   // Neon cyan
Color BLACK_MAIN  = {38, 20, 60, 255};    // Dark stone
Color BLACK_ACCENT= {180, 10, 255, 255};  // Electric purple
Color BASE_RING   = {50, 250, 200, 180};  // Cyan semi-transp for all bases

// 3D piece drawing implementation
void draw_piece3d(int piece, int row, int col, Camera camera, float anim_scale, float anim_alpha) {
    if (piece == EMPTY) return;
    
    Vector3 pos = board_to_world(row, col);
    pos.y = 0.5f; // Raise pieces above board
    
    Color piece_color;
    if (piece > 0) {
        piece_color = WHITE_MAIN;
    } else {
        piece_color = BLACK_MAIN;
    }
    
    // Apply animation alpha
    piece_color.a = (unsigned char)(piece_color.a * anim_alpha);
    
    // Scale for animation
    float scale = 0.3f * anim_scale;
    
    // Draw different shapes for different pieces
    int abs_piece = abs(piece);
    switch (abs_piece) {
        case W_PAWN:
            DrawSphere(pos, scale, piece_color);
            break;
        case W_ROOK:
            DrawCube(pos, scale * 2, scale * 2, scale * 2, piece_color);
            break;
        case W_KNIGHT:
            DrawCubeWires(pos, scale * 2, scale * 2, scale * 2, piece_color);
            break;
        case W_BISHOP:
            DrawCone(pos, scale, scale * 2, 8, piece_color);
            break;
        case W_QUEEN:
            DrawCylinder(pos, scale, scale, scale * 2, 8, piece_color);
            break;
        case W_KING:
            DrawCylinder(pos, scale * 1.2f, scale * 1.2f, scale * 2.5f, 8, piece_color);
            break;
    }
}

// --- Piece string names utility ---
const char* piece_name(int piece) {
    switch(piece) {
        case W_PAWN: return "W_PAWN";
        case W_ROOK: return "W_ROOK";
        case W_KNIGHT: return "W_KNIGHT";
        case W_BISHOP: return "W_BISHOP";
        case W_QUEEN: return "W_QUEEN";
        case W_KING: return "W_KING";
        case B_PAWN: return "B_PAWN";
        case B_ROOK: return "B_ROOK";
        case B_KNIGHT: return "B_KNIGHT";
        case B_BISHOP: return "B_BISHOP";
        case B_QUEEN: return "B_QUEEN";
        case B_KING: return "B_KING";
        default: return "EMPTY";
    }
}

// --- Board setup ---
void init_board(int board[8][8]) {
    for(int r=0;r<8;r++) for(int c=0;c<8;c++) board[r][c]=EMPTY;
    // Black
    board[0][0]=B_ROOK;   board[0][1]=B_KNIGHT; board[0][2]=B_BISHOP; board[0][3]=B_QUEEN;
    board[0][4]=B_KING;   board[0][5]=B_BISHOP; board[0][6]=B_KNIGHT; board[0][7]=B_ROOK;
    for(int c=0;c<8;c++) board[1][c]=B_PAWN;
    // White
    board[7][0]=W_ROOK;   board[7][1]=W_KNIGHT; board[7][2]=W_BISHOP; board[7][3]=W_QUEEN;
    board[7][4]=W_KING;   board[7][5]=W_BISHOP; board[7][6]=W_KNIGHT; board[7][7]=W_ROOK;
    for(int c=0;c<8;c++) board[6][c]=W_PAWN;
    printf("Board initialized (standard start)\n");
}

// Board to world: center board at origin (X,Z), squares 1.0f wide, Y=0 for base
Vector3 board_to_world(int row, int col) {
    float x = (float)col - 3.5f;
    float z = (float)row - 3.5f;
    return (Vector3){x, 0.0f, z};
}

// --- Piece Drawing Helpers ---
// (Same as milestone 1, omitted for brevity; see previous answer)
// [Use the exact same draw_piece3d implementation as Milestone 1]

// --- Mouse Picking Helper ---
// Returns true if the mouse ray hits the board and outputs the row/col (0..7), false otherwise
bool pick_tile(Camera camera, Vector2 mouse, int *out_row, int *out_col) {
    Ray ray = GetMouseRay(mouse, camera);
    // Plane Y=0 (board surface)
    if (fabsf(ray.direction.y) < 1e-4f) return false; // Parallel
    float t = -ray.position.y / ray.direction.y;
    if (t < 0.0f) return false; // Only in front
    Vector3 hit = Vector3Add(ray.position, Vector3Scale(ray.direction, t));
    // Board is from x,z = -3.5..+3.5
    float bx = hit.x + 3.5f;
    float bz = hit.z + 3.5f;
    int col = (int)floorf(bx + 0.5f);
    int row = (int)floorf(bz + 0.5f);
    if (row < 0 || row > 7 || col < 0 || col > 7) return false;
    *out_row = row;
    *out_col = col;
    return true;
}
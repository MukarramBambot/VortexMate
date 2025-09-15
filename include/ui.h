#pragma once
#include "raylib.h"
#include "db.h"

typedef struct {
    int game_state;
    int current_turn;
    char last_move[32];
    char message[64];
    float eval_score;
    int show_eval;
    char game_result[64]; // New: for game over overlay
    float logo_alpha;     // For fade-in
} UIOverlayInfo;

void draw_ui(const UIOverlayInfo *info, float logo_alpha);
void draw_game_result_overlay(const char *result);

// --- New function prototype ---
#ifndef UI_H
#define UI_H

#include "raylib.h"

void draw_logo_centered(Texture2D logo, bool logo_loaded, int x, int y, float alpha);

#endif


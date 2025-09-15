#include "ui.h"
#include <string.h>

// ... draw_ui as before ...
void draw_ui(const UIOverlayInfo *info, float logo_alpha) {
    // ... existing overlays ...
    // Draw version at bottom right
    DrawText("VortexMate v1.0", GetScreenWidth()-230, GetScreenHeight()-36, 26, GRAY);
}
// Centered game result overlay
void draw_game_result_overlay(const char *result) {
    int w = 600, h = 100;
    int x = GetScreenWidth()/2 - w/2, y = GetScreenHeight()/2 - h/2;
    DrawRectangle(x-8, y-8, w+16, h+16, (Color){0,0,0,170});
    DrawRectangle(x, y, w, h, (Color){32,24,54,220});
    DrawText(result, x+30, y+30, 36, YELLOW);
}

// --- New logo drawing function ---
void draw_logo_centered(Texture2D logo, bool logo_loaded, int x, int y, float alpha) {
    if (logo_loaded) {
        Color tint = WHITE;
        tint.a = (unsigned char)(alpha * 255);
        DrawTexture(logo, x, y, tint);
    }
}

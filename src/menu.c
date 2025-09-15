#include "menu.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// Menu button constants
#define BTN_W 300
#define BTN_H 60
#define BTN_SPACING 20

// Colors
#define MENU_BG_COLOR (Color){20, 20, 30, 200}
#define MENU_BTN_COLOR (Color){60, 60, 80, 255}
#define MENU_BTN_HOVER_COLOR (Color){80, 80, 100, 255}
#define MENU_TEXT_COLOR WHITE

// Helper function to draw a button
static bool menu_draw_button(const char *text, int x, int y, int w, int h, bool *hovered) {
    Vector2 mouse = GetMousePosition();
    bool is_hovered = (mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h);
    bool clicked = is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    
    if (hovered) *hovered = is_hovered;
    
    Color btn_color = is_hovered ? MENU_BTN_HOVER_COLOR : MENU_BTN_COLOR;
    DrawRectangle(x, y, w, h, btn_color);
    DrawRectangleLines(x, y, w, h, MENU_TEXT_COLOR);
    
    int text_width = MeasureText(text, 24);
    DrawText(text, x + (w - text_width) / 2, y + (h - 24) / 2, 24, MENU_TEXT_COLOR);
    
    return clicked;
}

MenuAction menu_main_draw() {
    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();
    int x = screen_w / 2 - BTN_W / 2;
    int y = screen_h / 2 - (5 * BTN_H + 4 * BTN_SPACING) / 2;
    
    // Background
    DrawRectangle(0, 0, screen_w, screen_h, MENU_BG_COLOR);
    
    // Title
    const char *title = "VortexMate Chess";
    int title_width = MeasureText(title, 48);
    DrawText(title, screen_w / 2 - title_width / 2, y - 100, 48, WHITE);
    
    MenuAction action = MENU_NONE;
    
    if (menu_draw_button("New Game vs AI", x, y, BTN_W, BTN_H, NULL))
        action = MENU_NEW_AI;
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("New Local Game", x, y, BTN_W, BTN_H, NULL))
        action = MENU_NEW_LOCAL;
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Host Multiplayer", x, y, BTN_W, BTN_H, NULL))
        action = MENU_HOST_MP;
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Join Multiplayer", x, y, BTN_W, BTN_H, NULL))
        action = MENU_JOIN_MP;
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Saved Games", x, y, BTN_W, BTN_H, NULL))
        action = MENU_SAVED_GAMES;
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Quit", x, y, BTN_W, BTN_H, NULL))
        action = MENU_QUIT;
    
    return action;
}

MenuAction menu_pause_draw() {
    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();
    int x = screen_w / 2 - BTN_W / 2;
    int y = screen_h / 2 - (4 * BTN_H + 3 * BTN_SPACING) / 2;
    
    // Semi-transparent background
    DrawRectangle(0, 0, screen_w, screen_h, (Color){0, 0, 0, 150});
    
    // Menu background
    int menu_w = BTN_W + 40;
    int menu_h = 4 * BTN_H + 3 * BTN_SPACING + 40;
    DrawRectangle(x - 20, y - 20, menu_w, menu_h, MENU_BG_COLOR);
    
    MenuAction action = MENU_NONE;
    
    if (menu_draw_button("Resume", x, y, BTN_W, BTN_H, NULL))
        action = MENU_RESUME;
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Save Game", x, y, BTN_W, BTN_H, NULL))
        action = MENU_SAVE;
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Resign", x, y, BTN_W, BTN_H, NULL))
        action = MENU_RESIGN;
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Quit to Main", x, y, BTN_W, BTN_H, NULL))
        action = MENU_QUIT_TO_MAIN;
    
    return action;
}

MenuAction menu_mp_setup_draw(char *ipbuf, int ipbuflen, int *port, bool is_join, bool *ip_entered) {
    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();
    int x = screen_w / 2 - BTN_W / 2;
    int y = screen_h / 2 - 150;
    
    DrawRectangle(0, 0, screen_w, screen_h, MENU_BG_COLOR);
    
    const char *title = is_join ? "Join Multiplayer Game" : "Host Multiplayer Game";
    int title_width = MeasureText(title, 36);
    DrawText(title, screen_w / 2 - title_width / 2, y - 80, 36, WHITE);
    
    if (is_join) {
        DrawText("Enter IP Address:", x, y, 24, WHITE);
        y += 30;
        DrawRectangle(x, y, BTN_W, 40, WHITE);
        DrawText(ipbuf, x + 10, y + 10, 20, BLACK);
        y += 60;
        
        // Simple text input handling
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125 && strlen(ipbuf) < ipbuflen - 1) {
                int len = strlen(ipbuf);
                ipbuf[len] = (char)key;
                ipbuf[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(ipbuf) > 0) {
            ipbuf[strlen(ipbuf) - 1] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            *ip_entered = true;
        }
    }
    
    DrawText("Port:", x, y, 24, WHITE);
    y += 30;
    DrawText(TextFormat("%d", *port), x, y, 20, WHITE);
    y += 40;
    
    MenuAction action = MENU_NONE;
    
    if (menu_draw_button(is_join ? "Connect" : "Start Server", x, y, BTN_W, BTN_H, NULL)) {
        if (is_join) {
            action = MENU_JOIN_MP;
        } else {
            action = MENU_HOST_MP;
        }
    }
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Back", x, y, BTN_W, BTN_H, NULL))
        action = MENU_QUIT_TO_MAIN;
    
    return action;
}

MenuAction menu_ai_difficulty_draw(AIDifficulty *out_difficulty) {
    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();
    int x = screen_w / 2 - BTN_W / 2;
    int y = screen_h / 2 - (3 * BTN_H + 2 * BTN_SPACING) / 2;
    
    DrawRectangle(0, 0, screen_w, screen_h, MENU_BG_COLOR);
    
    const char *title = "Select AI Difficulty";
    int title_width = MeasureText(title, 36);
    DrawText(title, screen_w / 2 - title_width / 2, y - 80, 36, WHITE);
    
    MenuAction action = MENU_NONE;
    
    if (menu_draw_button("Easy", x, y, BTN_W, BTN_H, NULL)) {
        *out_difficulty = AI_EASY;
        action = MENU_NEW_AI;
    }
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Medium", x, y, BTN_W, BTN_H, NULL)) {
        *out_difficulty = AI_MEDIUM;
        action = MENU_NEW_AI;
    }
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Hard", x, y, BTN_W, BTN_H, NULL)) {
        *out_difficulty = AI_HARD;
        action = MENU_NEW_AI;
    }
    y += BTN_H + BTN_SPACING;
    
    if (menu_draw_button("Back", x, y, BTN_W, BTN_H, NULL))
        action = MENU_QUIT_TO_MAIN;
    
    return action;
}

MenuAction menu_saved_games_draw() {
    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();
    
    DrawRectangle(0, 0, screen_w, screen_h, MENU_BG_COLOR);
    
    DrawText("Saved Games (replay coming soon)", 100, 120, 30, WHITE);
    
    if (menu_draw_button("Back", 100, 320, BTN_W, BTN_H, NULL))
        return MENU_QUIT_TO_MAIN;
    
    return MENU_NONE;
}

void menu_draw_overlay(MenuState state, Texture2D logo, bool loaded) {
    // This function can be used to draw overlays on top of the game
    // Implementation depends on specific needs
}
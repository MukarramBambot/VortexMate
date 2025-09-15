#pragma once
#include <stdbool.h>
#include "raylib.h"
#include "ai.h"

typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_INGAME,
    MENU_STATE_PAUSE,
    MENU_STATE_MP_SETUP,
    MENU_STATE_AI_DIFFICULTY,
    MENU_STATE_SAVED_GAMES
} MenuState;

typedef enum {
    MENU_NONE,
    MENU_NEW_AI,
    MENU_NEW_LOCAL,
    MENU_HOST_MP,
    MENU_JOIN_MP,
    MENU_LOAD,
    MENU_SAVED_GAMES,
    MENU_QUIT,
    MENU_RESUME,
    MENU_SAVE,
    MENU_RESIGN,
    MENU_QUIT_TO_MAIN
} MenuAction;

MenuAction menu_main_draw();
MenuAction menu_pause_draw();
MenuAction menu_mp_setup_draw(char *ipbuf, int ipbuflen, int *port, bool is_join, bool *ip_entered);
MenuAction menu_ai_difficulty_draw(AIDifficulty *out_difficulty);
MenuAction menu_saved_games_draw();
void menu_draw_overlay(MenuState state, Texture2D logo, bool loaded);
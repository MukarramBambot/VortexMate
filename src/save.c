#include "save.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// Save to plain text (expand to JSON if desired)
bool save_game(const SaveGameState *state, const char *filename)
{
    // Ensure saves directory exists
    #ifdef _WIN32
        _mkdir("saves");
    #else
        mkdir("saves", 0755);
    #endif
    FILE *f = fopen(filename, "w");
    if (!f) return false;
    // Save board
    for (int r=0; r<8; r++) {
        for (int c=0; c<8; c++)
            fprintf(f, "%d ", state->board[r][c]);
        fprintf(f, "\n");
    }
    fprintf(f, "%d\n", state->current_turn);
    fprintf(f, "%s\n", state->last_move);
    fprintf(f, "%s\n", state->message);
    fprintf(f, "%.2f\n", state->eval_score);
    fprintf(f, "%d\n", state->show_eval);
    fclose(f);
    printf("Game saved to %s\n", filename);
    return true;
}

bool load_game(SaveGameState *state, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) return false;
    for (int r=0; r<8; r++)
        for (int c=0; c<8; c++)
            fscanf(f, "%d", &state->board[r][c]);
    fscanf(f, "%d\n", &state->current_turn);
    fgets(state->last_move, sizeof(state->last_move), f); state->last_move[strcspn(state->last_move, "\n")] = 0;
    fgets(state->message, sizeof(state->message), f); state->message[strcspn(state->message, "\n")] = 0;
    fscanf(f, "%f\n", &state->eval_score);
    fscanf(f, "%d\n", &state->show_eval);
    fclose(f);
    printf("Game loaded from %s\n", filename);
    return true;
}
#pragma once
#include <stdbool.h>
#include <time.h>
#include <sqlite3.h>

// Result types
typedef enum { DB_WHITE_WIN, DB_BLACK_WIN, DB_DRAW, DB_RESIGN } DbResult;

typedef struct {
    int id;
    time_t date;
    char white[32];
    char black[32];
    char moves[1024]; // PGN
    DbResult result;
} DbGame;

// Open and close DB
bool db_open(const char *filename);
void db_close();

// Add a completed game (returns id or -1)
int db_add_game(const DbGame *game);

// List most recent games (returns count)
int db_list_games(DbGame *games, int max);

// Load game by id
bool db_load_game(int id, DbGame *out_game);
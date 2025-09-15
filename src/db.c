#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static sqlite3 *db = NULL;

bool db_open(const char *filename) {
    if (db) return true;
    
    if (sqlite3_open(filename, &db) != SQLITE_OK) {
        fprintf(stderr, "Warning: Could not open DB file (%s). Game history will not be saved.\n", filename);
        db = NULL;
        return false;
    }
    
    // Create tables if they don't exist
    const char *create_sql = 
        "CREATE TABLE IF NOT EXISTS games ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "date INTEGER,"
        "white TEXT,"
        "black TEXT,"
        "moves TEXT,"
        "result INTEGER"
        ");";
    
    char *err_msg = NULL;
    if (sqlite3_exec(db, create_sql, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        db = NULL;
        return false;
    }
    
    return true;
}

void db_close() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

int db_add_game(const DbGame *game) {
    if (!db) {
        fprintf(stderr, "Warning: DB unavailable, game not saved.\n");
        return -1;
    }
    
    const char *sql = "INSERT INTO games (date, white, black, moves, result) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int64(stmt, 1, (sqlite3_int64)game->date);
    sqlite3_bind_text(stmt, 2, game->white, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, game->black, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, game->moves, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, (int)game->result);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert game: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    return (int)sqlite3_last_insert_rowid(db);
}

int db_list_games(DbGame *games, int max) {
    if (!db) return 0;
    
    const char *sql = "SELECT id, date, white, black, moves, result FROM games ORDER BY date DESC LIMIT ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, max);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max) {
        games[count].id = sqlite3_column_int(stmt, 0);
        games[count].date = (time_t)sqlite3_column_int64(stmt, 1);
        strncpy(games[count].white, (const char*)sqlite3_column_text(stmt, 2), sizeof(games[count].white) - 1);
        strncpy(games[count].black, (const char*)sqlite3_column_text(stmt, 3), sizeof(games[count].black) - 1);
        strncpy(games[count].moves, (const char*)sqlite3_column_text(stmt, 4), sizeof(games[count].moves) - 1);
        games[count].result = (DbResult)sqlite3_column_int(stmt, 5);
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

bool db_load_game(int id, DbGame *out_game) {
    if (!db) return false;
    
    const char *sql = "SELECT id, date, white, black, moves, result FROM games WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        out_game->id = sqlite3_column_int(stmt, 0);
        out_game->date = (time_t)sqlite3_column_int64(stmt, 1);
        strncpy(out_game->white, (const char*)sqlite3_column_text(stmt, 2), sizeof(out_game->white) - 1);
        strncpy(out_game->black, (const char*)sqlite3_column_text(stmt, 3), sizeof(out_game->black) - 1);
        strncpy(out_game->moves, (const char*)sqlite3_column_text(stmt, 4), sizeof(out_game->moves) - 1);
        out_game->result = (DbResult)sqlite3_column_int(stmt, 5);
        found = true;
    }
    
    sqlite3_finalize(stmt);
    return found;
}
#include "raylib.h"
#include "config.h"
#include "db.h"
#include "ui.h"

int main(void) {
    // --- At startup: ---
    if (!DirectoryExists("assets")) MakeDirectory("assets");
    if (!DirectoryExists("saves")) MakeDirectory("saves");

    config_load("config.json");
    db_open("saves/vortexmate.db");

    InitWindow(1280, 720, "VortexMate");
    SetTargetFPS(60);

    Texture2D logo = LoadTexture("assets/VortexMate.png");
    bool logo_loaded = (logo.id > 0);

    // Menu / branding state
    float logo_alpha = 0.0f;
    bool in_menu = true;

    // --- Main Game Loop ---
    while (!WindowShouldClose()) {
        // --- Menu/branding polish: ---
        if (in_menu && logo_alpha < 1.0f) logo_alpha += GetFrameTime() * 1.2f;
        if (!in_menu && logo_alpha > 0.0f) logo_alpha -= GetFrameTime() * 1.2f;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (logo_loaded) {
            DrawTexture(logo, (GetScreenWidth() - logo.width) / 2, (GetScreenHeight() - logo.height) / 2, Fade(WHITE, logo_alpha)); // fade-in/fade-out
        }

        DrawText("VortexMate v1.0 © VortexGame",
                 GetScreenWidth() - 380,
                 GetScreenHeight() - 36,
                 26,
                 (Color){180, 255, 255, 90});

        // --- Replay mode (in saved games menu): ---
        // When a saved game is selected, allow ←/→ to step through the moves,
        // updating the board and overlays accordingly.
        // Show move x/N at top, and disable move/AI/network input during replay.

        // All overlays: DrawRectangle(x, y, w, h, (Color){0,0,0,160}) behind text for readability.

        EndDrawing();
    }

    // --- On exit: ---
    if (logo_loaded) UnloadTexture(logo);
    db_close();
    config_save("config.json");
    CloseWindow();

    return 0;
}
#pragma once
#include <stdbool.h>

typedef struct {
    int width, height;
    bool fullscreen;
    int ai_difficulty;
    float volume;
} VortexConfig;

extern VortexConfig vortex_config;

bool config_load(const char *filename);
bool config_save(const char *filename);
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

VortexConfig vortex_config = {1024, 768, false, 1, 1.0f};

bool config_load(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return false;
    char buf[256];
    while (fgets(buf, sizeof(buf), f)) {
        if (sscanf(buf, "width: %d", &vortex_config.width) == 1) continue;
        if (sscanf(buf, "height: %d", &vortex_config.height) == 1) continue;
        if (sscanf(buf, "fullscreen: %d", (int*)&vortex_config.fullscreen) == 1) continue;
        if (sscanf(buf, "ai_difficulty: %d", &vortex_config.ai_difficulty) == 1) continue;
        if (sscanf(buf, "volume: %f", &vortex_config.volume) == 1) continue;
    }
    fclose(f);
    return true;
}
bool config_save(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return false;
    fprintf(f, "width: %d\nheight: %d\nfullscreen: %d\nai_difficulty: %d\nvolume: %.2f\n",
        vortex_config.width, vortex_config.height, vortex_config.fullscreen,
        vortex_config.ai_difficulty, vortex_config.volume);
    fclose(f);
    return true;
}
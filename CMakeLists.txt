cmake_minimum_required(VERSION 3.10)
project(VortexMate C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build)

# Raylib (assume installed system-wide or via vcpkg/homebrew/apt)
find_package(raylib 4.0 REQUIRED)

# SQLite3 for DB
find_package(PkgConfig)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
include_directories(${SQLITE3_INCLUDE_DIRS})

add_executable(VortexMate
    src/main.c
    src/models.c
    src/chess_logic.c
    src/ai.c
    src/ui.c
    src/menu.c
    src/save.c
    src/network.c
    src/db.c
    src/config.c
)

# Sources
set(SOURCES
    src/main.c
    src/models.c
    src/chess_logic.c
    src/ai.c
    src/ui.c
    src/menu.c
    src/save.c
    src/network.c
    src/db.c
    src/config.c
)

include_directories(include)

target_link_libraries(VortexMate
    raylib
    m
    pthread
    dl
    rt
    GL
    X11
    Xi
    Xrandr
    Xxf86vm
    Xinerama
    Xcursor
${SQLITE3_LIBRARIES} m pthread dl)
if (APPLE)
    target_link_libraries(VortexMate "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()
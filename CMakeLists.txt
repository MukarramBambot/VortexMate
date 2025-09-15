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

# Sources
set(SOURCES
<<<<<<< HEAD
    main.c
    models.c
    chess_logic.c
    ai.c
    ui.c
    menu.c
    save.c
    network.c
    db.c
    config.c
)

=======
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

add_executable(VortexMate ${SOURCES})

target_link_libraries(VortexMate raylib ${SQLITE3_LIBRARIES} m pthread dl)
if (APPLE)
    target_link_libraries(VortexMate "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()
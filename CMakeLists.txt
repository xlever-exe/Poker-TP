cmake_minimum_required(VERSION 3.14)
project(Poker_TP C)

set(CMAKE_C_STANDARD 11)

# 1. Fetch raylib (Using 'master' fixes the CMake version error)
include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

# 2. Add ALL your source files so CLion compiles them
add_executable(Poker_TP
        cards.c
        cards.h
        dealcards.c
        logic.c
        game_loop.c)

# 3. Link raylib to your game
target_link_libraries(Poker_TP PRIVATE raylib)
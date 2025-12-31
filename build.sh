#!/bin/bash

# Check if SDL2 is installed in standard locations (example check, not exhaustive)
if [ ! -d "/usr/local/include/SDL2" ] && [ ! -d "/opt/homebrew/include/SDL2" ]; then
    echo "Warning: SDL2 include directory not found in standard paths."
    echo "This script assumes SDL2 is installed via brew. If compilation fails, check your SDL2 installation."
fi

# Attempt to find SDL2 paths (macOS typical paths)
SDL_CFLAGS=""
SDL_LIBS=""

if command -v pkg-config >/dev/null 2>&1; then
    SDL_CFLAGS=$(pkg-config --cflags sdl2)
    SDL_LIBS=$(pkg-config --libs sdl2)
else
    # Fallback manual paths for macOS (Homebrew Intel or Apple Silicon)
    if [ -d "/opt/homebrew/include/SDL2" ]; then
         SDL_CFLAGS="-I/opt/homebrew/include"
         SDL_LIBS="-L/opt/homebrew/lib -lSDL2"
    elif [ -d "/usr/local/include/SDL2" ]; then
         SDL_CFLAGS="-I/usr/local/include"
         SDL_LIBS="-L/usr/local/lib -lSDL2"
    else
        echo "Could not auto-detect SDL2. Using default assumption."
        SDL_LIBS="-lSDL2"
    fi
fi

echo "Compiling taquin.c..."
cmd="gcc taquin.c -o taquin $SDL_CFLAGS $SDL_LIBS"
echo $cmd
$cmd

if [ $? -eq 0 ]; then
    echo "Compilation successful! Run with: ./taquin"
else
    echo "Compilation failed."
fi

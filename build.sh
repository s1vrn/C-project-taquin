#!/bin/bash

# Check if SDL3 is installed in standard locations (example check, not exhaustive)
if [ ! -d "/usr/local/include/SDL3" ] && [ ! -d "/opt/homebrew/include/SDL3" ]; then
    echo "Warning: SDL3 include directory not found in standard paths."
    echo "This script assumes SDL3 is installed via brew or manually. If compilation fails, check your SDL3 installation."
fi

# Attempt to find SDL3 paths (macOS typical paths)
SDL_CFLAGS=""
SDL_LIBS=""

if command -v pkg-config >/dev/null 2>&1; then
    # Try getting flags for sdl3
    if pkg-config --exists sdl3; then
        SDL_CFLAGS=$(pkg-config --cflags sdl3)
        SDL_LIBS=$(pkg-config --libs sdl3)
    else
         echo "pkg-config could not find sdl3. Attempting manual fallback..."
    fi
fi

if [ -z "$SDL_LIBS" ]; then
    # Fallback manual paths for macOS (Homebrew Intel or Apple Silicon)
    if [ -d "/opt/homebrew/include/SDL3" ]; then
         SDL_CFLAGS="-I/opt/homebrew/include"
         SDL_LIBS="-L/opt/homebrew/lib -lSDL3"
    elif [ -d "/usr/local/include/SDL3" ]; then
         SDL_CFLAGS="-I/usr/local/include"
         SDL_LIBS="-L/usr/local/lib -lSDL3"
    else
        echo "Could not auto-detect SDL3 paths. Using default assumption."
        SDL_LIBS="-lSDL3"
    fi
fi

echo "Compiling taquin.c (SDL3)..."
cmd="gcc taquin.c -o taquin $SDL_CFLAGS $SDL_LIBS"
echo $cmd
$cmd

if [ $? -eq 0 ]; then
    echo "Compilation successful! Run with: ./taquin"
else
    echo "Compilation failed. Ensure SDL3 is installed and accessible."
fi

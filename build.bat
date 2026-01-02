@echo off
echo Compiling Taquin for Windows...
gcc taquin.c -o taquin.exe -lmingw32 -lSDL3
if %errorlevel% neq 0 (
    echo Compilation failed! Make sure GCC and SDL2 are installed and in your PATH.
    pause
    exit /b %errorlevel%
)
echo Compilation successful! Run taquin.exe to play.
pause

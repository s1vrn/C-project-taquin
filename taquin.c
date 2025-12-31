#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 4
#define TILE_SIZE 150
#define WINDOW_WIDTH (GRID_SIZE * TILE_SIZE)
#define WINDOW_HEIGHT (GRID_SIZE * TILE_SIZE)
#define GAP 5

typedef struct {
    int grid[GRID_SIZE][GRID_SIZE];
    int empty_x;
    int empty_y;
} GameState;

// Simple digit drawing segments for 0-9
// 7 segments: top, top-right, bottom-right, bottom, bottom-left, top-left, center
const int DIGIT_SEGMENTS[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

void init_game(GameState *game) {
    int count = 1;
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            game->grid[y][x] = count;
            count++;
        }
    }
    // Set the last cell as empty (0)
    game->grid[GRID_SIZE - 1][GRID_SIZE - 1] = 0;
    game->empty_x = GRID_SIZE - 1;
    game->empty_y = GRID_SIZE - 1;
}

void move_tile(GameState *game, int x, int y) {
    // Check if the move is valid (adjacent to the empty space)
    if ((abs(game->empty_x - x) == 1 && game->empty_y == y) || 
        (abs(game->empty_y - y) == 1 && game->empty_x == x)) {
        
        // Swap
        game->grid[game->empty_y][game->empty_x] = game->grid[y][x];
        game->grid[y][x] = 0;
        
        // Update empty position
        game->empty_x = x;
        game->empty_y = y;
    }
}

void shuffle_game(GameState *game) {
    // Perform random valid moves to shuffle
    for (int i = 0; i < 1000; i++) {
        int direction = rand() % 4;
        int target_x = game->empty_x;
        int target_y = game->empty_y;

        switch (direction) {
            case 0: target_y--; break; // Up
            case 1: target_y++; break; // Down
            case 2: target_x--; break; // Left
            case 3: target_x++; break; // Right
        }

        if (target_x >= 0 && target_x < GRID_SIZE && target_y >= 0 && target_y < GRID_SIZE) {
            move_tile(game, target_x, target_y);
        }
    }
}

bool check_win(GameState *game) {
    int count = 1;
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            // Last cell should be 0
            if (y == GRID_SIZE - 1 && x == GRID_SIZE - 1) {
                if (game->grid[y][x] != 0) return false;
            } else {
                if (game->grid[y][x] != count) return false;
                count++;
            }
        }
    }
    return true;
}

// Function to draw a number at a position using segments
void draw_number(SDL_Renderer *renderer, int number, int x, int y, int size) {
    int digits[2];
    int num_digits = 0;
    
    if (number >= 10) {
        digits[0] = number / 10;
        digits[1] = number % 10;
        num_digits = 2;
    } else {
        digits[0] = number;
        num_digits = 1;
    }

    int digit_width = size / 2;
    int digit_height = size;
    int stroke = size / 8;
    int start_x = x - (num_digits * digit_width + (num_digits - 1) * (stroke * 2)) / 2;
    int start_y = y - digit_height / 2;

    for (int i = 0; i < num_digits; i++) {
        int d = digits[i];
        int dx = start_x + i * (digit_width + stroke * 3);
        int dy = start_y;
        
        // Segments coordinates relative to dx, dy
        // Pts: 0:TL, 1:TR, 2:BR, 3:BL, 4:MidL, 5:MidR
        // Simpler approach: Draw rects for segments
        
        SDL_Rect segs[7];
        // Top
        segs[0] = (SDL_Rect){dx, dy, digit_width, stroke}; 
        // TR
        segs[1] = (SDL_Rect){dx + digit_width - stroke, dy, stroke, digit_height / 2}; 
        // BR
        segs[2] = (SDL_Rect){dx + digit_width - stroke, dy + digit_height / 2, stroke, digit_height / 2}; 
        // Bottom
        segs[3] = (SDL_Rect){dx, dy + digit_height - stroke, digit_width, stroke}; 
        // BL
        segs[4] = (SDL_Rect){dx, dy + digit_height / 2, stroke, digit_height / 2}; 
        // TL
        segs[5] = (SDL_Rect){dx, dy, stroke, digit_height / 2}; 
        // Center
        segs[6] = (SDL_Rect){dx, dy + digit_height / 2 - stroke / 2, digit_width, stroke};
        
        for (int s = 0; s < 7; s++) {
            if (DIGIT_SEGMENTS[d][s]) {
                SDL_RenderFillRect(renderer, &segs[s]);
            }
        }
    }
}


int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Taquin (15-Puzzle)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    GameState game;
    init_game(&game);
    shuffle_game(&game);

    bool running = true;
    SDL_Event e;
    bool has_won = false;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (!has_won) {
                    int x = e.button.x / TILE_SIZE;
                    int y = e.button.y / TILE_SIZE;
                    move_tile(&game, x, y);
                    if (check_win(&game)) {
                        has_won = true;
                        printf("You Won!\n");
                    }
                } else {
                    // Click to restart
                    shuffle_game(&game);
                    has_won = false;
                }
            } else if (e.type == SDL_KEYDOWN) {
                 if (!has_won) {
                    int target_x = game.empty_x;
                    int target_y = game.empty_y;
                    
                    switch(e.key.keysym.sym) {
                        case SDLK_UP: target_y++; break;    // Move tile UP into empty slot (empty moves down)
                        case SDLK_DOWN: target_y--; break;  // Move tile DOWN into empty slot (empty moves up)
                        case SDLK_LEFT: target_x++; break;  // Move tile LEFT into empty slot (empty moves right)
                        case SDLK_RIGHT: target_x--; break; // Move tile RIGHT into empty slot (empty moves left)
                        case SDLK_r: shuffle_game(&game); break;
                    }
                    
                    if (target_x >= 0 && target_x < GRID_SIZE && target_y >= 0 && target_y < GRID_SIZE) {
                        move_tile(&game, target_x, target_y);
                         if (check_win(&game)) {
                            has_won = true;
                            printf("You Won!\n");
                        }
                    }
                 } else {
                     if (e.key.keysym.sym == SDLK_r || e.key.keysym.sym == SDLK_RETURN) {
                         shuffle_game(&game);
                         has_won = false;
                     }
                 }
            }
        }

        // Clear screen
        if (has_won) {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Green if won
        } else {
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Dark background
        }
        SDL_RenderClear(renderer);

        // Draw Tiles
        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                int val = game.grid[y][x];
                if (val != 0) {
                    SDL_Rect rect = {x * TILE_SIZE + GAP, y * TILE_SIZE + GAP, TILE_SIZE - 2 * GAP, TILE_SIZE - 2 * GAP};
                    
                    // Simple gradient-ish effect by drawing concentric rects or just a flat color
                    // Tile Color
                    if (val % 2 == 0)
                        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
                    else 
                        SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255);
                        
                    SDL_RenderFillRect(renderer, &rect);
                    
                    // Draw Number
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    draw_number(renderer, val, 
                        rect.x + rect.w / 2, 
                        rect.y + rect.h / 2, 
                        TILE_SIZE / 3);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

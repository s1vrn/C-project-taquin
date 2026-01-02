#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_GRID_SIZE 5
#define TILE_SIZE 150
#define GAP 5
#define UI_HEIGHT 60

typedef struct {
    int grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
    int empty_x;
    int empty_y;
    int size; // Current grid size (3, 4, or 5)
} GameState;

typedef enum {
    STATE_MENU,
    STATE_PLAYING
} AppState;

// Simple digit drawing segments for 0-9
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

void init_game(GameState *game, int size) {
    if (size < 3) size = 3;
    if (size > MAX_GRID_SIZE) size = MAX_GRID_SIZE;
    
    game->size = size;
    int count = 1;
    for (int y = 0; y < game->size; y++) {
        for (int x = 0; x < game->size; x++) {
            game->grid[y][x] = count;
            count++;
        }
    }
    // Set the last cell as empty (0)
    game->grid[game->size - 1][game->size - 1] = 0;
    game->empty_x = game->size - 1;
    game->empty_y = game->size - 1;
}

void move_tile(GameState *game, int x, int y) {
    // Check bounds
    if (x < 0 || x >= game->size || y < 0 || y >= game->size) return;

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

        if (target_x >= 0 && target_x < game->size && target_y >= 0 && target_y < game->size) {
            move_tile(game, target_x, target_y);
        }
    }
}

bool check_win(GameState *game) {
    int count = 1;
    for (int y = 0; y < game->size; y++) {
        for (int x = 0; x < game->size; x++) {
            // Last cell should be 0
            if (y == game->size - 1 && x == game->size - 1) {
                if (game->grid[y][x] != 0) return false;
            } else {
                if (game->grid[y][x] != count) return false;
                count++;
            }
        }
    }
    return true;
}

// Function to draw a number at a position using segments (adapted for SDL3 FRect)
void draw_number(SDL_Renderer *renderer, int number, float x, float y, float size) {
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

    float digit_width = size / 2.0f;
    float digit_height = size;
    float stroke = size / 8.0f;
    if (stroke < 2.0f) stroke = 2.0f;
    
    float start_x = x - (num_digits * digit_width + (num_digits - 1) * (stroke * 2.0f)) / 2.0f;
    float start_y = y - digit_height / 2.0f;

    for (int i = 0; i < num_digits; i++) {
        int d = digits[i];
        float dx = start_x + i * (digit_width + stroke * 3.0f);
        float dy = start_y;
        
        SDL_FRect segs[7];
        // Top
        segs[0] = (SDL_FRect){dx, dy, digit_width, stroke}; 
        // TR
        segs[1] = (SDL_FRect){dx + digit_width - stroke, dy, stroke, digit_height / 2.0f}; 
        // BR
        segs[2] = (SDL_FRect){dx + digit_width - stroke, dy + digit_height / 2.0f, stroke, digit_height / 2.0f}; 
        // Bottom
        segs[3] = (SDL_FRect){dx, dy + digit_height - stroke, digit_width, stroke}; 
        // BL
        segs[4] = (SDL_FRect){dx, dy + digit_height / 2.0f, stroke, digit_height / 2.0f}; 
        // TL
        segs[5] = (SDL_FRect){dx, dy, stroke, digit_height / 2.0f}; 
        // Center
        segs[6] = (SDL_FRect){dx, dy + digit_height / 2.0f - stroke / 2.0f, digit_width, stroke};
        
        for (int s = 0; s < 7; s++) {
            if (DIGIT_SEGMENTS[d][s]) {
                SDL_RenderFillRect(renderer, &segs[s]);
            }
        }
    }
}

// Draw a simple Home icon
void draw_home_icon(SDL_Renderer *renderer, float x, float y, float w, float h) {
    // House body (square)
    SDL_FRect body = {x + w/4.0f, y + h/2.0f, w/2.0f, h/3.0f};
    SDL_RenderFillRect(renderer, &body);
    
    // Roof (triangle approximated with lines)
    float cx = x + w/2.0f;
    float top_y = y + h/6.0f;
    
    for(int i=0; i<w/2; i++) {
        if ((top_y + i) >= (y + h/2.0f)) break;
        SDL_RenderLine(renderer, cx - i, top_y + i, cx + i, top_y + i);
        // Fill gaps
        SDL_RenderLine(renderer, cx - i, top_y + i + 1, cx + i, top_y + i + 1);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Start with a reasonable default window size for the menu
    int w_width = 4 * TILE_SIZE;
    int w_height = 4 * TILE_SIZE + UI_HEIGHT;
    
    // SDL3: No x,y args
    SDL_Window *window = SDL_CreateWindow("Taquin (15-Puzzle) [SDL3]", w_width, w_height, 0); 
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // SDL3: CreateRenderer takes window and name (or NULL)
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    GameState game;
    AppState state = STATE_MENU;
    
    bool running = true;
    SDL_Event e;
    bool has_won = false;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_EVENT_QUIT) { // SDL3 constant
                running = false;
            } 
            else if (state == STATE_MENU) {
                if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    float mx = e.button.x;
                    float my = e.button.y;
                    
                    if (my > w_height/2 - 50 && my < w_height/2 + 50) {
                        int selected_size = 0;
                        if (mx < w_width / 3) selected_size = 3;
                        else if (mx < 2 * w_width / 3) selected_size = 4;
                        else selected_size = 5;
                        
                        init_game(&game, selected_size);
                        shuffle_game(&game);
                        state = STATE_PLAYING;
                        has_won = false;
                        
                        SDL_SetWindowSize(window, selected_size * TILE_SIZE, selected_size * TILE_SIZE + UI_HEIGHT);
                    }
                }
                else if (e.type == SDL_EVENT_KEY_DOWN) {
                   int selected_size = 0;
                   if (e.key.key == SDLK_3) selected_size = 3; // SDL3: e.key.key for keycode
                   else if (e.key.key == SDLK_4 ) selected_size = 4;
                   else if (e.key.key == SDLK_5) selected_size = 5;
                   
                   if (selected_size > 0) {
                        init_game(&game, selected_size);
                        shuffle_game(&game);
                        state = STATE_PLAYING;
                        has_won = false;
                        SDL_SetWindowSize(window, selected_size * TILE_SIZE, selected_size * TILE_SIZE + UI_HEIGHT);
                   }
                }
            }
            else if (state == STATE_PLAYING) {
                if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    float fx = e.button.x;
                    float fy = e.button.y;
                    int x = (int)(fx / TILE_SIZE);
                    int y = (int)(fy / TILE_SIZE);
                    
                    if (fy >= game.size * TILE_SIZE) {
                         state = STATE_MENU;
                         SDL_SetWindowSize(window, 4 * TILE_SIZE, 4 * TILE_SIZE + UI_HEIGHT);
                    } 
                    else if (!has_won) {
                        move_tile(&game, x, y);
                        if (check_win(&game)) {
                            has_won = true;
                            printf("You Won!\n");
                        }
                    } else {
                         if (e.button.button == SDL_BUTTON_RIGHT) {
                             state = STATE_MENU;
                             SDL_SetWindowSize(window, 4 * TILE_SIZE, 4 * TILE_SIZE + UI_HEIGHT);
                        } else {
                            shuffle_game(&game);
                            has_won = false;
                        }
                    }
                } else if (e.type == SDL_EVENT_KEY_DOWN) {
                     if (e.key.key == SDLK_ESCAPE) {
                         state = STATE_MENU;
                         SDL_SetWindowSize(window, 4 * TILE_SIZE, 4 * TILE_SIZE + UI_HEIGHT);
                     }
                     else if (!has_won) {
                        int target_x = game.empty_x;
                        int target_y = game.empty_y;
                        
                        switch(e.key.key) {
                            case SDLK_UP: target_y++; break;    
                            case SDLK_DOWN: target_y--; break;  
                            case SDLK_LEFT: target_x++; break;  
                            case SDLK_RIGHT: target_x--; break; 
                            case SDLK_r: shuffle_game(&game); break;
                        }
                        
                        if (target_x >= 0 && target_x < game.size && target_y >= 0 && target_y < game.size) {
                            move_tile(&game, target_x, target_y);
                             if (check_win(&game)) {
                                has_won = true;
                                printf("You Won!\n");
                            }
                        }
                     } else {
                         if (e.key.key == SDLK_r || e.key.key == SDLK_RETURN) {
                             shuffle_game(&game);
                             has_won = false;
                         }
                     }
                }
            }
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        if (state == STATE_MENU) {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            draw_number(renderer, 3, w/6.0f, h/2.0f, 100.0f);
            draw_number(renderer, 4, w/2.0f, h/2.0f, 100.0f);
             draw_number(renderer, 5, 5*w/6.0f, h/2.0f, 100.0f);
        }
        else if (state == STATE_PLAYING) {
            if (has_won) {
                SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); 
                SDL_FRect gameRect = {0.0f, 0.0f, (float)w, (float)(game.size * TILE_SIZE)};
                SDL_RenderFillRect(renderer, &gameRect);
            }
            
            for (int y = 0; y < game.size; y++) {
                for (int x = 0; x < game.size; x++) {
                    int val = game.grid[y][x];
                    if (val != 0) {
                        SDL_FRect rect = {
                            (float)(x * TILE_SIZE + GAP), 
                            (float)(y * TILE_SIZE + GAP), 
                            (float)(TILE_SIZE - 2 * GAP), 
                            (float)(TILE_SIZE - 2 * GAP)
                        };
                        
                        if (val % 2 == 0)
                            SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
                        else 
                            SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255);
                            
                        SDL_RenderFillRect(renderer, &rect);
                        
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        draw_number(renderer, val, 
                            rect.x + rect.w / 2.0f, 
                            rect.y + rect.h / 2.0f, 
                            TILE_SIZE / 3.0f);
                    }
                }
            }
            
            SDL_FRect uiRect = {0.0f, (float)(game.size * TILE_SIZE), (float)w, (float)UI_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &uiRect);
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            draw_home_icon(renderer, w/2.0f - 20.0f, game.size * TILE_SIZE + 10.0f, 40.0f, 40.0f);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

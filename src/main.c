// Pong SDL2 by Radon Therapy
// Initial version: 2014-10-08 05:11 - 2014-12-20 05:06
// Rewrite: 2023-06-06 19:38 - 

// Header files
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Variables and constants

// - Window settings
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FRAME_DELAY_MS (1000 / 60) // 60 FPS

// - Object settings
#define BALL_SIZE 30
#define BALL_DXY 5

#define PADDLE_W 20
#define PADDLE_H 100
#define PADDLE_DY 5

// - SDL variables
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

// - Game variables
bool gExit = false;
int playerScore = 0;
int opponentScore = 0;

// - Players and ball
typedef struct {
    int x, y;
    int dx, dy;
    int width, height;
} Object;

Object player, opponent, ball;

// Functions
bool game_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[error] Couldn't initialize SDL2: %s\n", SDL_GetError());
        return false;
    }

    if (TTF_Init() != 0) {
        printf("[error] Couldn't initialize SDL2_TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }

    gWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        printf("[error] Couldn't create window: %s\n", SDL_GetError());
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        printf("[error] Couldn't create renderer: %s\n", SDL_GetError());
        return false;
    }

    gFont = TTF_OpenFont("assets/BebasNeue-Regular.ttf", 28);
    if (!gFont) {
        printf("[error] Couldn't load font: %s\n", TTF_GetError());
        return false;
    }

    return true;
}

void game_exit()
{
    TTF_CloseFont(gFont);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    TTF_Quit();
    SDL_Quit();
}

void handle_input() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                gExit = true;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        player.dy = -PADDLE_DY;
                        break;

                    case SDLK_s:
                        player.dy = PADDLE_DY;
                        break;

                    case SDLK_UP:
                        opponent.dy = -PADDLE_DY;
                        break;

                    case SDLK_DOWN:
                        opponent.dy = PADDLE_DY;
                        break;

                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                    case SDLK_s:
                        player.dy = 0;
                        break;

                    case SDLK_UP:
                    case SDLK_DOWN:
                        opponent.dy = 0;
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}

void game_render()
{
    // Clean up screen
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    // Draw line
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawLine(gRenderer, WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT);

    // Draw objects
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(gRenderer, &(SDL_Rect){player.x, player.y, player.width, player.height});

    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(gRenderer, &(SDL_Rect){opponent.x, opponent.y, opponent.width, opponent.height});

    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(gRenderer, &(SDL_Rect){ball.x, ball.y, ball.width, ball.height});

    // Draw text
    SDL_Color textColor = { 255, 255, 255, 255 };

    char playerScoreText[4];
    char opponentScoreText[4];
    snprintf(playerScoreText, sizeof(playerScoreText), "%d", playerScore);
    snprintf(opponentScoreText, sizeof(opponentScoreText), "%d", opponentScore);

    SDL_Surface* playerSurface = TTF_RenderText_Solid(gFont, playerScoreText, textColor);
    SDL_Surface* opponentSurface = TTF_RenderText_Solid(gFont, opponentScoreText, textColor);

    SDL_Texture* playerTexture = SDL_CreateTextureFromSurface(gRenderer, playerSurface);
    SDL_Texture* opponentTexture = SDL_CreateTextureFromSurface(gRenderer, opponentSurface);

    SDL_Rect playerRect;
    SDL_Rect opponentRect;

    playerRect.x = WINDOW_WIDTH / 2 - 50;
    playerRect.y = 50;
    playerRect.w = playerSurface->w;
    playerRect.h = playerSurface->h;

    opponentRect.x = WINDOW_WIDTH / 2 + 50;
    opponentRect.y = 50;
    opponentRect.w = opponentSurface->w;
    opponentRect.h = opponentSurface->h;

    SDL_RenderCopy(gRenderer, playerTexture, NULL, &playerRect);
    SDL_RenderCopy(gRenderer, opponentTexture, NULL, &opponentRect);

    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(opponentTexture);
    SDL_FreeSurface(playerSurface);
    SDL_FreeSurface(opponentSurface);

    // Render
    SDL_RenderPresent(gRenderer);
}

bool check_collision(Object rect1, Object rect2) {
    if (rect1.x < rect2.x + rect2.width &&
        rect1.x + rect1.width > rect2.x &&
        rect1.y < rect2.y + rect2.height &&
        rect1.y + rect1.height > rect2.y) {
        return true; // Collision detected
    }

    return false; // No collision
}

// Main function
int main(int argc, char* argv[])
{
    if (!game_init()) {
        return 1;
    }

    // Initialize players and ball
    player = (Object) { 20, (WINDOW_HEIGHT - PADDLE_H) / 2, 0, 0, PADDLE_W, PADDLE_H };
    opponent = (Object) { WINDOW_WIDTH - PADDLE_W - 20, (WINDOW_HEIGHT - PADDLE_H) / 2, 0, 0, PADDLE_W, PADDLE_H };
    ball = (Object) { (WINDOW_WIDTH - BALL_SIZE) / 2, (WINDOW_HEIGHT -BALL_SIZE) / 2, BALL_DXY, BALL_DXY, BALL_SIZE, BALL_SIZE };

    // Initialize fps cap
    Uint32 frameStart, frameTime;

    while(!gExit) {
        // Set start time for fps cap
        frameStart = SDL_GetTicks();

        // Input handling
        handle_input();

        // Move players
        player.y += player.dy;
        opponent.y += opponent.dy;

        if (player.y < 0) {
            player.y = 0;
        } else if (player.y + PADDLE_H > WINDOW_HEIGHT) {
            player.y = WINDOW_HEIGHT - PADDLE_H;
        }

        if (opponent.y < 0) {
            opponent.y = 0;
        } else if (opponent.y + PADDLE_H > WINDOW_HEIGHT) {
            opponent.y = WINDOW_HEIGHT - PADDLE_H;
        }

        // Move ball
        ball.x += ball.dx;
        ball.y += ball.dy;

        if (ball.x < 0 || ball.x + ball.width > WINDOW_WIDTH) {
            ball.dx = -ball.dx;
            
            if (ball.x < 0) {
                opponentScore++;
            } else {
                playerScore++;
            }
        }
        
        if (ball.y < 0 || ball.y + ball.height > WINDOW_HEIGHT) {
            ball.dy = -ball.dy;
        }

        // Check paddle and ball collision
        if (check_collision(player, ball) || check_collision(opponent, ball)) {
            ball.dx = -ball.dx;
        }

        // Rendering
        game_render();

        // Delay
        frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY_MS > frameTime) {
            SDL_Delay(FRAME_DELAY_MS - frameTime);
        }
    }

    game_exit();
    return 0;
}
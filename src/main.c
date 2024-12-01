/*
 * Bong
 * Copyright (c) 2014-2024 Radon Therapy
 * This project is licensed under MIT License
 */

#include <stdio.h>
#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

/*
 * global variables
 */

SDL_Window *gameWindow = NULL;
SDL_Renderer *gameRenderer = NULL;

enum GAME_STATES {
	INTRO_STATE = 1U,
	MENU_STATE = 2U,
	GAME_STATE = 3U,
	PAUSE_STATE = 4U
};

unsigned int current_state = INTRO_STATE;

/*
 * function prototypes
 */

void intro_keyd_actions(SDL_Keycode);
void intro_render(void);

void menu_keyd_actions(SDL_Keycode);
void menu_render(void);

void ingame_keyd_actions(SDL_Keycode);
void ingame_render(void);

/*
 * game state functions
 */

void
intro_keyd_actions(SDL_Keycode key)
{
	if (key == SDLK_ESCAPE) {
		current_state = MENU_STATE;
	}
}

void
menu_keyd_actions(SDL_Keycode key)
{
	if (key == SDLK_ESCAPE) {
		current_state = GAME_STATE;
	}
}

void
ingame_keyd_actions(SDL_Keycode key)
{
	if (key == SDLK_ESCAPE) {
		current_state = INTRO_STATE;
	}
}

void
intro_render(void)
{
	SDL_FRect myRect = { 70, 70, 30, 75 };

	SDL_SetRenderDrawColor(gameRenderer, 255, 0, 0, 255);
	SDL_RenderFillRect(gameRenderer, &myRect);
}

void
menu_render(void)
{
	SDL_FRect myRect = { 70, 70, 30, 75 };

	SDL_SetRenderDrawColor(gameRenderer, 0, 255, 0, 255);
	SDL_RenderFillRect(gameRenderer, &myRect);
}

void
ingame_render(void)
{
	SDL_FRect myRect = { 70, 70, 30, 75 };

	SDL_SetRenderDrawColor(gameRenderer, 0, 0, 255, 255);
	SDL_RenderFillRect(gameRenderer, &myRect);
}

/*
 * sdl callbacks
 */

SDL_AppResult
SDL_AppInit(void **appstate, int argc, char **argv)
{
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	gameWindow = SDL_CreateWindow("Bong", 800, 500, SDL_WINDOW_OPENGL);

	if (gameWindow == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window init failed!\n");
		return SDL_APP_FAILURE;
	}

	gameRenderer = SDL_CreateRenderer(gameWindow, "OpenGL");

	if (gameRenderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer init failed!\n");
		return SDL_APP_FAILURE;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate(void *appstate)
{
	// we clear screen here so next render won't be written on top of that
	SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gameRenderer);

	// here we try to navigate app to different states
	switch (current_state) {
		case INTRO_STATE:
			intro_render();
			break;

		case MENU_STATE:
			menu_render();
			break;

		case GAME_STATE:
			ingame_render();
			break;

		default:
			break;
	}

	SDL_RenderPresent(gameRenderer);

	return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void *appstate, SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}

	if (event->type == SDL_EVENT_KEY_DOWN) {
		switch (current_state) {
			case INTRO_STATE:
				intro_keyd_actions(event->key.key);
				break;

			case MENU_STATE:
				menu_keyd_actions(event->key.key);
				break;

			case GAME_STATE:
				ingame_keyd_actions(event->key.key);
				break;

			default:
				break;
		}
	}

	return SDL_APP_CONTINUE;
}

void
SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	SDL_DestroyRenderer(gameRenderer);
	SDL_DestroyWindow(gameWindow);
}

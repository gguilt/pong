#include <stdio.h>
#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_Window *gameWindow = NULL;
SDL_Renderer *gameRenderer = NULL;

// PART FOR BB

/*
SDL_Texture*
BB_CreateBMPTexture(char *location)
{
	SDL_Texture *texture = NULL;
	SDL_Surface *surface = NULL;
	
	surface = SDL_LoadBMP(location);
	texture = SDL_CreateTextureFromSurface(gameRenderer, surface);

	SDL_DestroySurface(surface);

	return &texture;
}
*/

// PART FOR GAME
//
//



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
	//SDL_SetRenderDrawColor(gameRenderer, 255, 255, 255, 255);
	//SDL_RenderFillRect(gameRenderer, &myRect);
	//
	//
	//
	
	SDL_FRect myRect = { 15, 35, 50, 75 };

	SDL_SetRenderDrawColor(gameRenderer, 25, 255, 25, 255);
	SDL_RenderClear(gameRenderer);

	SDL_SetRenderDrawColor(gameRenderer, 255, 25, 25, 255);
	SDL_RenderFillRect(gameRenderer, &myRect);

	SDL_RenderPresent(gameRenderer);

	return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void *appstate, SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

void
SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	SDL_DestroyRenderer(gameRenderer);
	SDL_DestroyWindow(gameWindow);
}

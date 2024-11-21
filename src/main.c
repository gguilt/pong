#include <stdio.h>
#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_Window *gameWindow = NULL;

SDL_AppResult
SDL_AppInit(void **appstate, int argc, char **argv)
{
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	gameWindow = SDL_CreateWindow("Bong", 300, 400, SDL_WINDOW_OPENGL);

	if (gameWindow == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window init failed!\n");
		return 1;
	}
	return 0;
}

SDL_AppResult
SDL_AppIterate(void *appstate)
{
	return 0;
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
	SDL_DestroyWindow(gameWindow);
}

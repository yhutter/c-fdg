#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>

SDL_Window * window = NULL;
int window_width = 1280;
int window_height = 720;
Uint64 start_time = 0;
Uint64 current_time = 0;

SDL_Renderer* renderer = NULL;


// Main Entry Point of the Application
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("ERROR: Failed to initialize SDL because of '%s'", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    window = SDL_CreateWindow("FDG", window_width, window_height, 0);
    if (window == NULL) {
        SDL_Log("ERROR: Failed to create SDL window because of '%s'", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        SDL_Log("ERROR: Failed to create SDL renderer because of '%s'", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    start_time = SDL_GetTicks();
    return SDL_APP_CONTINUE;
}

// Called every tick
SDL_AppResult SDL_AppIterate(void *appstate) {
    current_time = SDL_GetTicks();
    Uint64 delta_time = current_time - start_time;

    SDL_SetRenderDrawColor(renderer, 0x18, 0x18, 0x18, 0xff);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);

    start_time = current_time;
    return SDL_APP_CONTINUE;
}

// Process any events
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch(event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        default:
            return SDL_APP_CONTINUE;
    }
}

// Shutdown
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


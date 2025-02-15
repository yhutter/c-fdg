#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_events.h>

#include "HandmadeMath.h"
#include "array.h"

#include <stdlib.h>
#include <stdbool.h>

/////////////////////////
// Struct definitions
/////////////////////////
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} color_t;

typedef struct {
    HMM_Vec2 pos;
    HMM_Vec2 acc;
    HMM_Vec2 vel;
    color_t color;
    float size;
} blob_t;

typedef struct {
    float length;
    blob_t* b1;
    blob_t* b2;
} spring_t;


/////////////////////////
// Global Variables
/////////////////////////

#define BACKGROUND_COLOR (color_t){0x18, 0x18, 0x18, 0xff}
#define FOREGROUND_COLOR (color_t){0xff, 0xdd, 0x33, 0xff}
#define BLOB_SIZE 5
#define K 0.005f
#define SPAWN_RATE_BLOBS 500
#define SPRING_LENGTH 250
#define BLOB_MAX_SPEED 5.0f
#define BLOB_DRAG 0.98f
#define FPS 60
#define FRAME_TARGET_TIME (1000.0f / FPS)

SDL_Window * window = NULL;
int window_width = 1280;
int window_height = 720;
Uint64 start_time = 0;
Uint64 current_time = 0;
bool running = false;

SDL_Renderer* renderer = NULL;

blob_t* blobs = NULL;
spring_t* springs = NULL;

/////////////////////////
// Functions
/////////////////////////
blob_t create_blob(HMM_Vec2 pos, float size, color_t color) {
    blob_t blob = {
        .pos = pos,
        .acc = HMM_V2(0.0f, 0.0f),
        .vel = HMM_V2(0.0f, 0.0f),
        .size = size,
        .color = color
    };
    return blob;
}

spring_t create_spring(float length, blob_t* b1, blob_t* b2) {
    spring_t spring = {
        .length = length,
        .b1 = b1,
        .b2 = b2
    };
    return spring;
}

void accelerate_blob(blob_t* blob, HMM_Vec2 f) {
    // f = m * a -> a = f / m (note that mass is 1
    blob->acc = HMM_AddV2(blob->acc, f);
}

static inline HMM_Vec2 vec2_clamp(HMM_Vec2 v, float min, float max) {
    if (HMM_LenV2(v) > 0.0f) {
        float clamped_mag = HMM_Clamp(HMM_LenV2(v), min, max);
        v = HMM_NormV2(v);
        v = HMM_MulV2F(v, clamped_mag);
    }
    return v;
}

void spring_move_blobs(spring_t* s) {
    HMM_Vec2 f = HMM_SubV2(s->b2->pos, s->b1->pos);
    float mag = K * (HMM_LenV2(f) -  s->length);
    f = vec2_clamp(f, 0.0, mag);
    accelerate_blob(s->b1, f);
    f = HMM_MulV2F(f, -1.0f);
    accelerate_blob(s->b2, f);
}


void update_blob_position(blob_t* blob) {
    blob->vel = HMM_AddV2(blob->vel, blob->acc);
    blob->vel = HMM_MulV2F(blob->vel, BLOB_DRAG);
    blob->vel = vec2_clamp(blob->vel, 0.0f, BLOB_MAX_SPEED);
    blob->pos = HMM_AddV2(blob->pos, blob->vel);
    blob->acc = HMM_V2(0.0f, 0.0f);
}

void render_blob(blob_t* blob) {
    uint8_t r = blob->color.r;
    uint8_t g = blob->color.g;
    uint8_t b = blob->color.b;
    uint8_t a = blob->color.a;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    SDL_FRect rect = {
        .x = blob->pos.X,
        .y = blob->pos.Y,
        .w = blob->size,
        .h = blob->size,
    };
    SDL_RenderFillRect(renderer, &rect);
}

/////////////////////////
// Game Loop Functions
/////////////////////////
bool initialize(void) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("ERROR: Failed to initialize SDL because of '%s'", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("FDG", window_width, window_height, 0);
    if (window == NULL) {
        SDL_Log("ERROR: Failed to create SDL window because of '%s'", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        SDL_Log("ERROR: Failed to create SDL renderer because of '%s'", SDL_GetError());
        return false;
    }
    start_time = SDL_GetTicks();

    springs = NULL;
    blobs = NULL;
    
    // Create some blobs
    for (int i = 0; i < SPAWN_RATE_BLOBS; i++) {
        float random_x = 10 + rand() % 1270;
        float random_y = 10 + rand() % 710;
        HMM_Vec2 pos = HMM_V2(random_x, random_y);
        blob_t blob = create_blob(pos, BLOB_SIZE, FOREGROUND_COLOR);
        array_push(blobs, blob);
    }

    // Connect springs to blobs
    for (int i = 0; i < array_length(blobs); i++) {
        for (int j = i + 1; j < array_length(blobs); j++) {
            blob_t* b1 = &blobs[i];
            blob_t* b2 = &blobs[j];
            spring_t spring = create_spring(SPRING_LENGTH, b1, b2);
            array_push(springs, spring);
        }
    }
    return true;
}

void process(void) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN: 
                if (e.key.key ==  SDLK_ESCAPE) {
                    running = false;
                    break;
                }
        }
    }
}

void update(void) {
    current_time = SDL_GetTicks();
    Uint64 delta_time = current_time - start_time;

    int time_to_wait = FRAME_TARGET_TIME - delta_time;

    // Delay execution if we are too fast (telling the OS that it can perform other tasks in the meantime by using SDL_Delay)
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    SDL_Log("Delta Ms: is %llu\n", delta_time);
    for (int i = 0; i < array_length(springs); i++) {
        spring_t* spring = &springs[i];
        spring_move_blobs(spring);
    }

    for (int i = 0; i < array_length(blobs); i++) {
        blob_t* blob = &blobs[i];
        update_blob_position(blob);
    }
    start_time = current_time;
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0x18, 0x18, 0x18, 0xff);
    SDL_RenderClear(renderer);
    for (int i = 0; i < array_length(blobs); i++) {
        blob_t* blob = &blobs[i];
        render_blob(blob);
    }
    SDL_RenderPresent(renderer);
}

void shutdown(void) {
    array_free(springs);
    array_free(blobs);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(void) {
    running = initialize();
    while(running) {
        process();
        update();
        render();
    }
    shutdown();
    return 0;
}


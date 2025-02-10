#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "array.h"

#define BACKGROUND_COLOR (Color){0x18, 0x18, 0x18, 0xff}
#define FOREGROUND_COLOR (Color){0xff, 0xdd, 0x33, 0xff}
#define BLOB_SIZE 5
#define K 0.005f
#define SPAWN_RATE_BLOBS 500
#define SPRING_LENGTH 250
#define BLOB_MAX_SPEED 5.0f
#define BLOB_DRAG 0.98f

typedef struct {
    Vector2 pos;
    Vector2 acc;
    Vector2 vel;
    Color color;
    float size;
} blob_t;

typedef struct {
    float length;
    blob_t* b1;
    blob_t* b2;
} spring_t;

blob_t* blobs = NULL;
spring_t* springs = NULL;


spring_t create_spring(float length, blob_t* b1, blob_t* b2) {
    spring_t spring = {
        .length = length,
        .b1 = b1,
        .b2 = b2
    };
    return spring;
}

void accelerate_blob(blob_t* blob, Vector2 f) {
    // f = m * a -> a = f / m (note that mass is 1
    blob->acc = Vector2Add(blob->acc, f);
}

void spring_move_blobs(spring_t* s) {
    Vector2 f = Vector2Subtract(s->b2->pos, s->b1->pos);
    float mag = K * (Vector2Length(f) - s->length);
    f = Vector2ClampValue(f, 0.0, mag);

    accelerate_blob(s->b1, f);
    f = Vector2Scale(f, -1.0f);
    accelerate_blob(s->b2, f);
}


blob_t create_blob(Vector2 pos, float size, Color color) {
    blob_t blob = {
        .pos = pos,
        .acc = {0.0f, 0.0f},
        .vel = {0.0f, 0.0f},
        .size = size,
        .color = color
    };
    return blob;
}


void update_blob_position(blob_t* blob) {
    blob->vel = Vector2Add(blob->vel, blob->acc);
    blob->vel = Vector2Scale(blob->vel, BLOB_DRAG);
    blob->vel = Vector2ClampValue(blob->vel, 0.0f, BLOB_MAX_SPEED);
    blob->pos = Vector2Add(blob->pos, blob->vel);
    blob->acc = Vector2Zero();
}

void render_blob(blob_t* blob) {
    DrawRectangleV(blob->pos, (Vector2) {blob->size, blob->size}, blob->color);
}

void spawn_blobs_and_springs(int num_blobs) {
    springs = NULL;
    blobs = NULL;
    // Create some blobs
    for (int i = 0; i < num_blobs; i++) {
        Vector2 pos = {
            .x = GetRandomValue(10, 1270),
            .y = GetRandomValue(10, 710)
        };
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
}


int main(void) {
    InitWindow(1280, 720, "FDG");
    SetTargetFPS(60);

    Camera2D camera = {0};
    camera.zoom = 1.0f;
    int num_total_blobs = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);

            if(IsKeyPressed(KEY_D)) {
                num_total_blobs += SPAWN_RATE_BLOBS;
                spawn_blobs_and_springs(num_total_blobs);
            }

            BeginMode2D(camera);
                for (int i = 0; i < array_length(springs); i++) {
                    spring_t* spring = &springs[i];
                    spring_move_blobs(spring);
                }

                for (int i = 0; i < array_length(blobs); i++) {
                    blob_t* blob = &blobs[i];
                    update_blob_position(blob);
                    render_blob(blob);
                }
            EndMode2D();
            DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 24, FOREGROUND_COLOR);
        EndDrawing();
    }
    CloseWindow();
    array_free(blobs);
    array_free(springs);
    return 0;
}

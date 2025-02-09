#include "raylib.h"
#include "raymath.h"

#define BACKGROUND_COLOR (Color){0x18, 0x18, 0x18, 0xff}
#define BLOB_COLOR (Color){0xff, 0xdd, 0x33, 0xff}

typedef struct {
    Vector2 pos;
    Vector2 acc;
    Vector2 vel;
    Color color;
    float size;
} blob_t;


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

void accelerate_blob(blob_t* blob, Vector2 f) {
    // f = m * a -> a = f / m (note that mass is 1
    blob->acc = Vector2Add(blob->acc, f);
}

void update_blob_position(blob_t* blob) {
    blob->vel = Vector2Add(blob->vel, blob->acc);
    blob->pos = Vector2Add(blob->pos, blob->vel);
    blob->acc = Vector2Zero();
}

void render_blob(blob_t* blob) {
    DrawCircleV(blob->pos, blob->size, blob->color);
}


int main(void) {
    InitWindow(1280, 720, "FDG");
    SetTargetFPS(60);

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    blob_t blob = create_blob((Vector2){ 640.0f, 320.f }, 10.0f, BLOB_COLOR);


    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            BeginMode2D(camera);
                accelerate_blob(&blob, (Vector2){0.0, 0.9f});
                update_blob_position(&blob);
                render_blob(&blob);
            EndMode2D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

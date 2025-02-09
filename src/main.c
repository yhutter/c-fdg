#include "raylib.h"
#include "raymath.h"

#define BACKGROUND_COLOR (Color){0x18, 0x18, 0x18, 0xff}
#define BLOB_COLOR (Color){0xff, 0xdd, 0x33, 0xff}
#define K 0.005f

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
    f = Vector2Scale(f, mag);
    f = Vector2Normalize(f);

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

    blob_t blob1 = create_blob((Vector2){ 320.0f, 320.f }, 10.0f, BLOB_COLOR);
    blob_t blob2 = create_blob((Vector2){ 620.0f, 320.f }, 10.0f, BLOB_COLOR);
    spring_t spring = create_spring(150.0f, &blob1, &blob2);


    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            BeginMode2D(camera);
                spring_move_blobs(&spring);
                update_blob_position(&blob1);
                update_blob_position(&blob2);
                render_blob(&blob1);
                render_blob(&blob2);
            EndMode2D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

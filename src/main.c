#include "raylib.h"

#define BACKGROUND_COLOR (Color){24, 24, 24}

int main(void) {
    InitWindow(1280, 720, "FDG");

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

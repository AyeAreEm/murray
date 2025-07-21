#include <string.h>
#include "defs.h"
#include "raylib.h"

void gameover_draw(State state) {
    BeginDrawing();
    ClearBackground(BLACK);

    float font_size = 50.0f;
    DrawText("Game Over", WIDTH / 2 - font_size * strlen("Game Over") / 4, HEIGHT / 2 - font_size, font_size, RED);

    // TODO: add play again button

    EndDrawing();
}

void gameover_update(State *state) {
    // TODO: add play again button
}

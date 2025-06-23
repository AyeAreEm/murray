#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "include/dyn.h"
#include "defs.h"
#include "include/exrand.h"

bool debug_mode = false;

int main() {
    exrand_random_seed();

    InitWindow(WIDTH, HEIGHT, "Murray");
    SetTargetFPS(60);
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    Menu menu = menu_init();
    State state = (State){
        .kind = StateMenu,
        .menu = menu,
        .running = true,
    };

    while (!WindowShouldClose() && state.running) {
        switch (state.kind) {
        case StateMenu:
            menu_draw(state);
            menu_update(&state);
            break;
        case StateGameOver:
            gameover_draw(state);
            gameover_update(&state);
            break;
        case StatePlay:
            game_draw(state);
            game_update(&state);
            break;
        }
    }

    if (state.kind == StatePlay) {
        for (usize i = 0; i < dynlen(state.play.textures); i++) {
            UnloadTexture(state.play.textures[i]);
        }
    }

    CloseWindow();
}

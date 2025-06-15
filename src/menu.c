#include "defs.h"
#include "raylib.h"
#include <string.h>

Menu menu_init() {
    float button_w = 150.0f;
    float button_h = 50.0f;

    Button play = (Button){
        .shape = (Rectangle){
            .x = WIDTH / 2 - button_w / 2,
            .y = HEIGHT / 2 - button_h / 2,
            .width = button_w,
            .height = button_h,
        },
        .text = "Play",
    };

    Button quit = (Button){
        .shape = (Rectangle){
            .x = WIDTH / 2 - button_w / 2,
            .y = HEIGHT / 2 + button_h * 1.5f,
            .width = button_w,
            .height = button_h,
        },
        .text = "Quit",
    };

    Menu menu = (Menu){
        .play = play,
        .quit = quit,
    };

    return menu;
}

void menu_draw(State state) {
    Menu menu = state.menu;

    BeginDrawing();
    ClearBackground(BLACK);

    float title_font_size = 50.0f;
    DrawText("Murray", WIDTH / 2 - title_font_size * strlen("Murray") / 4, HEIGHT / 2 - title_font_size * 4, title_font_size, WHITE);

    float button_font_size = 20.0f;
    DrawRectangleRec(menu.play.shape, WHITE);
    float play_text_x = (menu.play.shape.x + menu.play.shape.width / 2) - button_font_size * strlen(menu.play.text) / 4;
    float play_text_y = (menu.play.shape.y + menu.play.shape.height / 2) - button_font_size / 2;
    DrawText(menu.play.text, play_text_x, play_text_y, button_font_size, BLACK);

    DrawRectangleRec(menu.quit.shape, WHITE);
    float quit_text_x = (menu.quit.shape.x + menu.quit.shape.width / 2) - button_font_size * strlen(menu.quit.text) / 4;
    float quit_text_y = (menu.quit.shape.y + menu.quit.shape.height / 2) - button_font_size / 2;
    DrawText(menu.quit.text, quit_text_x, quit_text_y, button_font_size, BLACK);

    EndDrawing();
}

void menu_update(State *state) {
    Menu *menu = &state->menu;

    if (CheckCollisionPointRec(GetMousePosition(), menu->play.shape)) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            state->kind = StatePlay;
            state->play = game_init();
        }
    } else if (CheckCollisionPointRec(GetMousePosition(), menu->quit.shape)) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            state->running = false;
        }
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
}

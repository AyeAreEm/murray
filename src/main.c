#include <raylib.h>
#include <raymath.h>
#include "include/dyn.h"
#include "defs.h"

dynimpl(Zombie, Zombie);
dynimpl(Bullet, Bullet);

const Gun glock = {
    .kind = GunPistol,
    .name = "Glock",

    .fire_rate = 5,
    .max_mag = 24,
    .max_reserve = 240,
    .bullet_health = 1,

    .mag = 24,
    .reserve = 240,
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Murray");
    SetTargetFPS(60);
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    Texture player_stand_tex = LoadTexture("./src/assets/player_stand.png");
    Texture zombie_stand_tex = LoadTexture("./src/assets/zombie_stand.png");

    Player player = {
        .shape = {
            .x = WIDTH/2,
            .y = HEIGHT/2,
            .width = PLAYER_WIDTH,
            .height = PLAYER_HEIGHT,
        },
        .tex = player_stand_tex,
        .speed = PLAYER_WALK,
        .gun = glock,
    };

    Camera2D camera = {
        .target = {player.shape.x, player.shape.y},
        .offset = {player.shape.x, player.shape.y},
        .rotation = 0.0f,
        .zoom = 1.0f,
    };
    player.camera = camera;

    Zombie *zombies = NULL;
    dynpushZombie(&zombies, (Zombie){
        .shape = ((Rectangle){
            .x = WIDTH,
            .y = HEIGHT,
            .width = ZOMBIE_WIDTH,
            .height = ZOMBIE_HEIGHT,
        }),
        .speed = ZOMBIE_START_SPEED,
        .tex = zombie_stand_tex,
    });
    dynpushZombie(&zombies, (Zombie){
        .shape = {
            .x = 0,
            .y = 0,
            .width = ZOMBIE_WIDTH,
            .height = ZOMBIE_HEIGHT,
        },
        .speed = ZOMBIE_START_SPEED,
        .tex = zombie_stand_tex,
    });

    Game game = {
        .player = player,
        .zombies = zombies,
        .bullets = NULL,
    };

    while (!WindowShouldClose()) {
        game_draw(game);
        game_update(&game);
    }

    UnloadTexture(player_stand_tex);
    UnloadTexture(zombie_stand_tex);
    CloseWindow();
}

#include <stdio.h>
#include <stddef.h>
#include <raylib.h>
#include <raymath.h>
#include "include/dyn.h"

#define WIDTH 800.0f
#define HEIGHT 600.0f

#define ENTITY_SIZE 32.0f

#define PLAYER_WALK 3.0f
#define PLAYER_RUN 6.0f

#define ZOMBIE_START_SPEED 2.0f

typedef struct Player {
    Rectangle shape;
    float speed;
} Player;

typedef struct Zombie {
    Rectangle shape;
    Texture tex;
    float speed;
} Zombie;
dynimpl(Zombie, Zombie);

typedef struct Game {
    Player player;
    Dyn(Zombie) zombies;
} Game;

void player_draw(Player self) {
    DrawRectangleRec(self.shape, GREEN);
}

void player_update(Player *self) {
    Vector2 pos = {0};

    if (IsKeyDown(KEY_W)) {
        pos.y -= 1.0;
    }
    if (IsKeyDown(KEY_A)) {
        pos.x -= 1.0;
    }
    if (IsKeyDown(KEY_S)) {
        pos.y += 1.0;
    }
    if (IsKeyDown(KEY_D)) {
        pos.x += 1.0;
    }

    if (pos.x != 0 && pos.y != 0) {
        pos = Vector2Normalize(pos);
    }

    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        self->speed = PLAYER_RUN;
    } else {
        self->speed = PLAYER_WALK;
    }

    self->shape.x += pos.x * self->speed;
    self->shape.y += pos.y * self->speed;
}

void zombie_draw(Zombie self) {
    // DrawTextureEx(self.tex, (Vector2){self.shape.x, self.shape.y}, 0.0f, 2.0f, WHITE);
    DrawTexture(self.tex, self.shape.x, self.shape.y, WHITE);
}

void zombie_update(Zombie *self, Player player) {
    Vector2 direction = {
        .x = player.shape.x - self->shape.x,
        .y = player.shape.y - self->shape.y,
    };
    direction = Vector2Normalize(direction);

    self->shape.x += direction.x * self->speed;
    self->shape.y += direction.y * self->speed;

    if (CheckCollisionRecs(self->shape, player.shape)) {
        printf("you should be dead lol\n");
    }
}

void game_draw(Game self) {
    BeginDrawing();
    ClearBackground(BLACK);

    player_draw(self.player);
    for (size_t i = 0; i < dynlen(self.zombies); i++) {
        zombie_draw(self.zombies[i]);
    }

    EndDrawing();
}

void game_update(Game *self) {
    player_update(&self->player);

    // O(n^2) booooo
    for (size_t i = 0; i < dynlen(self->zombies); i++) {
        zombie_update(&self->zombies[i], self->player);

        for (size_t j = 0; j < dynlen(self->zombies); j++) {
            if (j == i) {
                break;
            }

            if (CheckCollisionRecs(self->zombies[j].shape, self->zombies[i].shape)) {
                float overlap_x;
                if (self->zombies[j].shape.x < self->zombies[i].shape.x) {
                    overlap_x = (self->zombies[j].shape.x + ENTITY_SIZE) - self->zombies[i].shape.x;
                } else {
                    overlap_x = (self->zombies[i].shape.x + ENTITY_SIZE) - self->zombies[j].shape.x;
                }

                float overlap_y;
                if (self->zombies[j].shape.y < self->zombies[i].shape.y) {
                    overlap_y = (self->zombies[j].shape.y + ENTITY_SIZE) - self->zombies[i].shape.y;
                } else {
                    overlap_y = (self->zombies[i].shape.y + ENTITY_SIZE) - self->zombies[j].shape.y;
                }

                if (overlap_x < overlap_y) {
                    if (self->zombies[j].shape.x < self->zombies[i].shape.x) {
                        self->zombies[j].shape.x -= overlap_x;
                    } else {
                        self->zombies[j].shape.x += overlap_x;
                    }
                } else {
                    if (self->zombies[j].shape.y < self->zombies[i].shape.y) {
                        self->zombies[j].shape.y -= overlap_y;
                    } else {
                        self->zombies[j].shape.y += overlap_y;
                    }
                }
            }
        }
    }
}

int main() {
    InitWindow(WIDTH, HEIGHT, "Murray");
    SetTargetFPS(60);
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    Texture zombie_stand_tex = LoadTexture("./src/assets/zombie_stand.png");

    Player player = {
        .shape = {
            .x = WIDTH/2,
            .y = HEIGHT/2,
            .width = ENTITY_SIZE,
            .height = ENTITY_SIZE,
        },
        .speed = PLAYER_WALK,
    };

    Zombie *zombies = NULL;
    dynpushZombie(&zombies, (Zombie){
        .shape = ((Rectangle){
            .x = WIDTH,
            .y = HEIGHT,
            .width = ENTITY_SIZE,
            .height = ENTITY_SIZE,
        }),
        .speed = ZOMBIE_START_SPEED,
        .tex = zombie_stand_tex,
    });
    dynpushZombie(&zombies, (Zombie){
        .shape = {
            .x = 0,
            .y = 0,
            .width = ENTITY_SIZE,
            .height = ENTITY_SIZE,
        },
        .speed = ZOMBIE_START_SPEED,
        .tex = zombie_stand_tex,
    });

    Game game = {
        .player = player,
        .zombies = zombies,
    };

    while (!WindowShouldClose()) {
        game_draw(game);
        game_update(&game);
    }

    UnloadTexture(zombie_stand_tex);
    CloseWindow();
}

#include "include/dyn.h"
#include "defs.h"
#include "raylib.h"

dynimpl(Zombie, Zombie);
dynimpl(Bullet, Bullet);
dynimpl(Texture, Texture);

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

Game game_init() {
    Texture player_stand_tex = LoadTexture("./src/assets/player_stand.png");
    Texture zombie_stand_tex = LoadTexture("./src/assets/zombie_stand.png");

    Texture *textures = NULL;
    dynpush(Texture, &textures, player_stand_tex);
    dynpush(Texture, &textures, zombie_stand_tex);

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
        .health = 3,
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
        .textures = textures,
    };

    return game;
}

void game_draw(State state) {
    Game self = state.play;

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(self.player.camera);

    if (self.bullets != NULL) {
        for (size_t i = 0; i < dynlen(self.bullets); i++) {
            bullet_draw(self.bullets[i]);
        }
    }

    if (self.zombies != NULL) {
        for (size_t i = 0; i < dynlen(self.zombies); i++) {
            zombie_draw(self.zombies[i]);
        }
    }

    player_draw(self.player);

    EndMode2D();
    EndDrawing();
}

void game_update(State *state) {
    Game *self = &state->play;

    player_update(state);

    if (self->bullets != NULL) {
        for (size_t i = 0; i < dynlen(self->bullets); i++) {
            bool remove = bullet_update(&self->bullets[i], self->player);
            if (remove) {
                dynremove(Bullet, self->bullets, i);
            }

            for (size_t j = 0; j < dynlen(self->zombies); j++) {
                if (CheckCollisionRecs(self->bullets[i].shape, self->zombies[j].shape)) {
                    dynremove(Zombie, self->zombies, j);

                    self->bullets[i].health -= 1;
                    if (self->bullets[i].health == 0) {
                        dynremove(Bullet, self->bullets, i);
                    }
                }
            }
        }
    }

    if (self->zombies == NULL) {
        return;
    }

    // O(n^2) booooo
    for (size_t i = 0; i < dynlen(self->zombies); i++) {
        zombie_update(&self->zombies[i], state);

        for (size_t j = 0; j < dynlen(self->zombies); j++) {
            if (j == i) {
                break;
            }

            if (CheckCollisionRecs(self->zombies[j].shape, self->zombies[i].shape)) {
                float overlap_x;
                if (self->zombies[j].shape.x < self->zombies[i].shape.x) {
                    overlap_x = (self->zombies[j].shape.x + self->zombies[j].shape.width) - self->zombies[i].shape.x;
                } else {
                    overlap_x = (self->zombies[i].shape.x + self->zombies[i].shape.width) - self->zombies[j].shape.x;
                }

                float overlap_y;
                if (self->zombies[j].shape.y < self->zombies[i].shape.y) {
                    overlap_y = (self->zombies[j].shape.y + self->zombies[j].shape.height) - self->zombies[i].shape.y;
                } else {
                    overlap_y = (self->zombies[i].shape.y + self->zombies[i].shape.height) - self->zombies[j].shape.y;
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

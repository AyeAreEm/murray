#include <string.h>
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
        .max_health = 3,
        .health = 3,
        .in_iframe = false,
        .iframe_time = 0.0,
    };

    Camera2D camera = {
        .target = {player.shape.x, player.shape.y},
        .offset = {player.shape.x, player.shape.y},
        .rotation = 0.0f,
        .zoom = 1.0f,
    };
    player.camera = camera;

    Game game = {
        .player = player,
        .zombies_per_round = 6,
        .bullets = NULL,
        .textures = {
            [TexturePlayer] = player_stand_tex,
            [TextureZombie] = zombie_stand_tex,
        },
        .round = 1,
        .round_transition = 0.0,
        .in_round_transition = false,
    };

    Zombie *zombies = NULL;

    for (usize i = 0; i < game.zombies_per_round; i++) {
        dynpush(Zombie, &zombies, zombie_spawn(game, zombie_stand_tex));
    }

    game.zombies = zombies;

    return game;
}

void draw_hud(Game game) {
    Player player = game.player;

    // round
    {
        const char* round = TextFormat("%d", game.round);
        u32 round_len = strlen(round);

        float font_size = 35.0f;
        float x = WIDTH - ((float)round_len) * font_size;
        float y = font_size / 2.0f;
        DrawTextEx(GetFontDefault(), round, (Vector2){x, y}, font_size, 1.0f, RED);
    }

    // health
    {
        float width = player.health * 20.0f;
        float height = 15.0f;
        float x = 20.0f;
        float y = height;

        DrawRectangleRec((Rectangle){x, y, width, height}, RED);
        DrawRectangleLinesEx((Rectangle){x, y, player.max_health * 20.0f, height}, 1.0f, WHITE);
    }

    // gun ammo and stuff
    {
        char buffer[11];
        int buffer_len = 11;
        sprintf(buffer, "%d / %d", player.gun.mag, player.gun.reserve);

        float font_size = 20.0f;
        float x = WIDTH - ((float)buffer_len / 2.0f) * font_size;
        float y = HEIGHT - font_size;
        DrawTextEx(GetFontDefault(), buffer, (Vector2){x, y}, font_size, 1.0f, WHITE);

        DrawTextEx(GetFontDefault(), player.gun.name, (Vector2){x, y + font_size}, font_size, 1.5f, WHITE);
    }
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
    draw_hud(self);

    EndDrawing();
}

void game_update(State *state) {
    Game *self = &state->play;

    if (IsKeyPressed(KEY_PERIOD)) {
        debug_mode = !debug_mode;
    }

    player_update(state);

    if (self->bullets != NULL) {
        for (size_t i = 0; i < dynlen(self->bullets); i++) {
            bool remove = bullet_update(&self->bullets[i], self->player);
            if (remove) {
                dynremove(Bullet, self->bullets, i);
            }

            // kill zombie lol
            for (size_t j = 0; j < dynlen(self->zombies); j++) {
                if (CheckCollisionRecs(self->bullets[i].shape, self->zombies[j].shape)) {
                    self->zombies[j].health -= 1;
                    if (self->zombies[j].health == 0) {
                        dynremove(Zombie, self->zombies, j);
                    }

                    self->bullets[i].health -= 1;
                    if (self->bullets[i].health == 0) {
                        dynremove(Bullet, self->bullets, i);
                    }
                }
            }
        }
    }

    if (dynlen(self->zombies) == 0) {
        if (!self->in_round_transition) {
            self->in_round_transition = true;
            self->round_transition = GetTime();
        }

        double time_since_round_transition = GetTime() - self->round_transition;
        if (self->in_round_transition && time_since_round_transition > 5.0) {
            self->round += 1;
            self->in_round_transition = false;
            self->zombies_per_round *= 1.5;
            printf("zombies per round: %zu\n", self->zombies_per_round);

            for (usize i = 0; i < self->zombies_per_round; i++) {
                dynpush(Zombie, &self->zombies, zombie_spawn(*self, self->textures[TextureZombie]));
            }
        }
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

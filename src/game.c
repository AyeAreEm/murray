#include <stdio.h>
#include <math.h>
#include <string.h>
#include "include/dyn.h"
#include "defs.h"
#include "raylib.h"
#include "raymath.h"

dynimpl(Zombie, Zombie);
dynimpl(Bullet, Bullet);
dynimpl(Texture, Texture);

Game game_init() {
    Texture player_stand_tex = LoadTexture("./src/assets/murray_player_stand.png");
    Texture zombie_stand_tex = LoadTexture("./src/assets/murray_zombie_stand.png");

    Player player = player_init(player_stand_tex);

    Camera2D camera = {
        .target = {player.shape.x, player.shape.y},
        .offset = {player.shape.x, player.shape.y},
        .rotation = 0.0f,
        .zoom = 1.0f,
    };
    player.camera = camera;

    Game game = {
        .player = player,
        .zombie_speed = ZOMBIE_START_SPEED,
        .zombies_per_round = 6,
        .zombie_health = ZOMBIE_BASE_HEALTH,
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

    // zombies left
    {
        const char* zombies_left = TextFormat("Zombies: %zu", dynlen(game.zombies));
        int font_size = 15;
        DrawText(zombies_left, WIDTH / 2.0f - (float)font_size * strlen(zombies_left) / 4, font_size, font_size, WHITE);
    }

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
        const char* buffer = TextFormat("%d / %d", player.gun.mag, player.gun.reserve);

        float font_size = 20.0f;
        float x = WIDTH - ((float)strlen(buffer) + 1.0f) / 2.0f * font_size;
        float y = HEIGHT - font_size;

        DrawTextEx(GetFontDefault(), buffer, (Vector2){x, y}, font_size, 1.0f, WHITE);
        DrawTextEx(GetFontDefault(), player.gun.name, (Vector2){x, y + font_size}, font_size, 1.5f, WHITE);
    }

    // points
    {
        const char *points = TextFormat("%lu", player.points);
        float font_size = 20.0f;
        float x = font_size / 2.0f;
        float y = HEIGHT;
        DrawTextEx(GetFontDefault(), points, (Vector2){x, y}, font_size, 1.0f, WHITE);
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
        for (size_t i = 0; i < dynlen(self->bullets);) {
            bool remove = bullet_update(&self->bullets[i], self->player);
            if (remove) {
                dynremove(Bullet, self->bullets, i);
                continue;
            }

            // hurt and kill zombies.
            // handle points
            bool removed_bullet = false;
            for (size_t j = 0; j < dynlen(self->zombies);) {
                if (CheckCollisionRecs(self->bullets[i].shape, self->zombies[j].shape)) {
                    self->zombies[j].health = (u32)Clamp((i32)(self->zombies[j].health - self->bullets[i].damage), 0.0f, self->zombies[j].health);

                    if (self->zombies[j].health == 0) {
                        // zombie dead, remove zombie, add 100 points
                        self->player.points += 100;
                        dynremove(Zombie, self->zombies, j);
                        continue;
                    }

                    // zombie lives, add 10 points, remove bullet
                    self->player.points += 10;
                    dynremove(Bullet, self->bullets, i);
                    removed_bullet = true;
                    j += 1;
                    continue;
                }

                j += 1;
            }

            if (removed_bullet) {
                continue;
            }

            i += 1;
        }
    }

    // round transition
    {
        if (dynlen(self->zombies) == 0) {
            if (!self->in_round_transition) {
                self->in_round_transition = true;
                self->round_transition = GetTime();
            }

            double time_since_round_transition = GetTime() - self->round_transition;
            if (self->in_round_transition && time_since_round_transition > 5.0) {
                self->round += 1;
                self->in_round_transition = false;

                self->zombie_speed = Clamp(self->zombie_speed + 0.2f, 0.0f, ZOMBIE_MAX_SPEED);

                self->zombies_per_round = (usize)roundf(self->zombies_per_round * 1.2f);
                self->zombie_health *= 1.5;

                for (usize i = 0; i < self->zombies_per_round; i++) {
                    dynpush(Zombie, &self->zombies, zombie_spawn(*self, self->textures[TextureZombie]));
                }
            }
        }
    }

    // O(n^2) booooo
    for (size_t i = 0; i < dynlen(self->zombies);) {
        zombie_update(&self->zombies[i], state);

        // check if zombie has been meleed
        if (self->player.melee_area_active && CheckCollisionRecs(self->zombies[i].shape, self->player.melee_area)) {
            self->zombies[i].health = (u32)Clamp((i32)(self->zombies[i].health - MELEE_DMG), 0.0f, self->zombies[i].health);
            if (self->zombies[i].health == 0) {
                dynremove(Zombie, self->zombies, i);
                continue;
            }
        }

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

        i += 1;
    }
}

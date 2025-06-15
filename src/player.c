#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include "include/dyn.h"
#include "defs.h"

void player_hud(Player self) {
    char buffer[11];
    int buffer_len = 11;
    sprintf(buffer, "%d / %d", self.gun.mag, self.gun.reserve);

    float font_size = 20.0f;
    float ammo_x = self.shape.x + ((WIDTH - (float)buffer_len * font_size) - (WIDTH / 2.0f)) + font_size * 5.0f;
    float ammo_y = self.shape.y + ((HEIGHT - font_size) - (HEIGHT / 2.0f)) - font_size * 2.0f;
    DrawTextEx(GetFontDefault(), buffer, (Vector2){ammo_x, ammo_y}, font_size, 1.0f, WHITE);

    DrawTextEx(GetFontDefault(), self.gun.name, (Vector2){ammo_x, ammo_y + font_size}, font_size, 1.5f, WHITE);
}

void player_draw(Player self) {
    DrawTextureEx(self.tex, (Vector2){self.shape.x, self.shape.y}, 0.0f, 2.0f, WHITE);

    player_hud(self);
}

Bullet player_spawn_bullet(Player *self) {
    Bullet bullet = {
        .shape = {
            .width = BULLET_WIDTH,
            .height = BULLET_HEIGHT,
            .x = self->shape.x + (self->shape.width / 2.0f),
            .y = self->shape.y + (self->shape.height / 2.0f),
        },
        .health = self->gun.bullet_health,
    };

    Vector2 mouse_pos = MOUSE_XY(*self);
    Vector2 direction = Vector2Subtract(mouse_pos, (Vector2){self->shape.x, self->shape.y});
    bullet.direction = Vector2Normalize(direction);
    return bullet;
}

Bullet player_fire_pistol(Player *self) {
    self->gun.mag -= 1;
    return player_spawn_bullet(self);
}

Bullet player_fire(Player *self) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return (Bullet){0};
    }

    if (self->gun.mag == 0) {
        return (Bullet){0};
    }

    switch (self->gun.kind) {
    case GunPistol:
        return player_fire_pistol(self);
        break;
    }
}

void player_movement(Player *self) {
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

    self->camera.target = (Vector2){self->shape.x, self->shape.y};
}

void player_reload(Player *self) {
    if (IsKeyPressed(KEY_R)) {
        u32 mag_needs = self->gun.max_mag - self->gun.mag;
        u32 moved = MIN(mag_needs, self->gun.reserve);
        self->gun.reserve -= moved;
        self->gun.mag += moved;
    }
}

void player_get_hit(State *state) {
    Player *player = &state->play.player;

    player->health -= 1;

    if (player->health == 0) {
        state->kind = StateGameOver;
    }
}

void player_update(State *state) {
    Player *player = &state->play.player;

    player_movement(player);
    player_reload(player);

    Bullet bullet = player_fire(player);
    if (bullet.shape.width == 0 && bullet.shape.height == 0) {
        return;
    }
    dynpush(Bullet, &state->play.bullets, bullet);
}

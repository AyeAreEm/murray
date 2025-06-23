#include <raylib.h>
#include <raymath.h>
#include "include/dyn.h"
#include "defs.h"

void player_draw(Player self) {
    DrawTextureEx(self.tex, (Vector2){self.shape.x - self.shape.width / 2.0f, self.shape.y}, 0.0f, 1.0f, WHITE);

    // line between player and mouse position
    if (debug_mode) {
        DrawRectangleLinesEx(self.melee_area, 1.0f, SKYBLUE);
        DrawRectangleLinesEx(self.shape, 1.0f, BLUE);
        DrawLineEx(
            (Vector2){
                self.shape.x + self.shape.width / 2.0f,
                self.shape.y + self.shape.height / 2.0f,
            },
            MOUSE_XY(self),
            1.0f,
            BLUE
        );
    }
}

Bullet player_spawn_bullet(Player *self) {
    Bullet bullet = {
        .shape = {
            .x = self->shape.x + (self->shape.width / 2.0f),
            .y = self->shape.y + (self->shape.height / 2.0f),
            .width = BULLET_WIDTH,
            .height = BULLET_HEIGHT,
        },
        .health = self->gun.bullet_health,
    };

    Vector2 direction = (Vector2){
        .x = MOUSE_X(*self) - bullet.shape.x,
        .y = MOUSE_Y(*self) - bullet.shape.y,
    };
    bullet.direction = Vector2Normalize(direction);

    return bullet;
}

Bullet player_fire_gun(Player *self) {
    if (debug_mode) {
        return player_spawn_bullet(self);
    }

    self->gun.mag -= 1;
    return player_spawn_bullet(self);
}

Bullet player_fire(Player *self) {
    if (self->gun.mag == 0) {
        return (Bullet){0};
    }

    switch (self->gun.kind) {
    case GunSemi:
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return player_fire_gun(self);
        }
        return (Bullet){0};
    case GunFull:
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            return player_fire_gun(self);
        }
        return (Bullet){0};
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
    self->melee_area.x = self->shape.x - PLAYER_MELEE_WIDTH / 2.0f;
    self->melee_area.y = self->shape.y - PLAYER_MELEE_HEIGHT / 2.0f;

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

    if (player->in_iframe) {
        return;
    }

    player->in_iframe = true;
    player->iframe_time = GetTime();

    player->regen_time = GetTime();
    player->health -= 1;

    if (player->health == 0) {
        state->kind = StateGameOver;
    }
}

void player_melee(Player *self) {
    if (!IsKeyPressed(KEY_V)) {
        self->melee_area_active = false;
        return;
    }

    self->melee_area_active = true;
}

void player_update(State *state) {
    Player *player = &state->play.player;

    player_movement(player);
    player_reload(player);

    double time_since_hit = GetTime() - player->iframe_time;
    if (player->in_iframe && time_since_hit > 0.5) {
        player->in_iframe = false;
    }

    double time_since_regen = GetTime() - player->regen_time;
    if (time_since_regen > 3.0 && player->health != player->max_health) {
        player->regen_time = GetTime();
        player->health += 1;
    }

    player_melee(player);

    Bullet bullet = player_fire(player);
    if (bullet.shape.width == 0 && bullet.shape.height == 0) {
        return;
    }
    dynpush(Bullet, &state->play.bullets, bullet);
}

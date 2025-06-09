#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "include/dyn.h"
#include "defs.h"

void player_draw(Player self) {
    DrawTextureEx(self.tex, (Vector2){self.shape.x, self.shape.y}, 0.0f, 2.0f, WHITE);
}

Bullet player_spawn_bullet(Player *self) {
    Bullet bullet = {
        .shape = {
            .width = BULLET_WIDTH,
            .height = BULLET_HEIGHT,
            .x = self->shape.x + (self->shape.width / 2.0f),
            .y = self->shape.y + (self->shape.height / 2.0f),
        },
    };

    Vector2 mouse_pos = GetMousePosition();
    printf("mouse x: %.1f, y: %.1f\n", mouse_pos.x, mouse_pos.y);
    bullet.direction = Vector2Normalize(mouse_pos);
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

void player_update(Game *game) {
    player_movement(&game->player);
    Bullet bullet = player_fire(&game->player);
    if (bullet.shape.width == 0 && bullet.shape.height == 0) {
        return;
    }
    dynpush(Bullet, &game->bullets, bullet);
}

#include <stdlib.h>
#include "defs.h"
#include "include/exrand.h"
#include "raylib.h"

Zombie zombie_spawn(Game game, Texture tex) {
    Player player = game.player;

    float x;
    if ((double)rand() / (double)RAND_MAX >= 0.5) {
        x = exrand_range((int)(player.shape.x * 2.0f), (int)(player.shape.x * 2.5f));
    } else {
        x = exrand_range((int)-player.shape.x, (int)(-player.shape.x * 1.5f));
    }

    float y;
    if ((double)rand() / (double)RAND_MAX >= 0.5) {
        y = exrand_range((int)(player.shape.y * 2.5f), (int)(player.shape.y * 2.0f));
    } else {
        y = exrand_range((int)-player.shape.y, (int)(-player.shape.y * 1.5f));
    }

    Zombie zombie = {
        .shape = {
            .x = x,
            .y = y,
            .width = ZOMBIE_WIDTH,
            .height = ZOMBIE_HEIGHT,
        },
        .speed = ZOMBIE_START_SPEED,
        .max_health = ZOMBIE_BASE_HEALTH,
        .health = ZOMBIE_BASE_HEALTH,
        .tex = tex,
    };

    return zombie;
}

void zombie_draw(Zombie self) {
    DrawTextureEx(self.tex, (Vector2){self.shape.x, self.shape.y}, 0.0f, 1.0f, WHITE);

    // health bar
    {
        float width = self.health * 8.0f;
        float height = 7.0f;

        float max_width = self.max_health * 8.0f;
        float x = self.shape.x + self.shape.width / 2.0f - max_width / 2.0f;
        float y = self.shape.y + self.shape.height + height;

        DrawRectangleRec((Rectangle){x, y, width, height}, RED);
    }

    if (debug_mode) {
        DrawRectangleLinesEx(self.shape, 1.0f, BLUE);
    }
}

void zombie_update(Zombie *self, State *state) {
    Player *player = &state->play.player;

    Vector2 direction = {
        .x = player->shape.x - self->shape.x,
        .y = player->shape.y - self->shape.y,
    };
    direction = Vector2Normalize(direction);

    self->shape.x += direction.x * self->speed;
    self->shape.y += direction.y * self->speed;

    if (CheckCollisionRecs(self->shape, player->shape)) {
        player_get_hit(state);
    }
}

#include <stdlib.h>
#include "defs.h"
#include "raylib.h"

Zombie zombie_spawn(Game game, Texture tex) {
    // TODO: change this to the player's view size
    float x;
    if ((double)rand() / (double)RAND_MAX >= 0.5) {
        // fmod(rand(), (WIDTH * 1.5f - WIDTH + 1.0f) + WIDTH);
        x = (float)(rand() % (int)(WIDTH * 1.5f - WIDTH + 1.0f) + WIDTH);
    } else {
        x = (float)(rand() % (int)-WIDTH + 1);
    }

    // TODO: change this to the player's view size
    float y;
    if ((double)rand() / (double)RAND_MAX >= 0.5) {
        y = (float)(rand() % (int)(HEIGHT * 1.5f - HEIGHT + 1.0f) + HEIGHT);
    } else {
        y = (float)(rand() % (int)-HEIGHT + 1);
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
    DrawTextureEx(self.tex, (Vector2){self.shape.x, self.shape.y}, 0.0f, 2.0f, WHITE);

    float width = self.health * 8.0f;
    float height = 7.0f;

    float max_width = self.max_health * 8.0f;
    float x = self.shape.x + max_width / 4.0f;
    float y = self.shape.y + self.shape.height + height;

    DrawRectangleRec((Rectangle){x, y, width, height}, RED);
    // DrawRectangleLinesEx((Rectangle){x, y, max_width, height}, 1.0f, WHITE);
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

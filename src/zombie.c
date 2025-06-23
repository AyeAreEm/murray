#include <stdlib.h>
#include "defs.h"
#include "include/exrand.h"
#include "raylib.h"

Zombie zombie_spawn(Game game, Texture tex) {
    Player player = game.player;

    int safe_box = 80;

    float x;
    if ((double)rand() / (double)RAND_MAX >= 0.5) {
        x = exrand_range((int)player.shape.x + safe_box, (int)(player.shape.x + WIDTH / 2.0f));
    } else {
        x = exrand_range((int)(player.shape.x - WIDTH / 2.0f), (int)player.shape.x - safe_box);
    }

    float y;
    if ((double)rand() / (double)RAND_MAX >= 0.5) {
        y = exrand_range((int)player.shape.y + safe_box, (int)(player.shape.y + HEIGHT / 2.0f));
    } else {
        y = exrand_range((int)(player.shape.y - HEIGHT / 2.0f), (int)player.shape.y - safe_box);
    }

    Zombie zombie = {
        .state = {
            .kind = ZombieStateSpawn,
            .spawn_time = GetTime(),
        },
        .shape = {
            .x = x,
            .y = y,
            .width = ZOMBIE_WIDTH,
            .height = ZOMBIE_HEIGHT,
        },
        .speed = game.zombie_speed,
        .max_health = game.zombie_health,
        .health = game.zombie_health,
        .tex = tex,
    };

    return zombie;
}

void zombie_draw(Zombie self) {
    DrawTextureEx(self.tex, (Vector2){self.shape.x - self.shape.width / 2.0f, self.shape.y}, 0.0f, 1.0f, WHITE);

    // health bar
    {

        float max_width = 40.0f;

        float width = ((float)self.health / (float)self.max_health) * max_width;
        float height = 7.0f;

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

    switch (self->state.kind) {
        case ZombieStateSpawn: {
            if (GetTime() - self->state.spawn_time > 3.0) {
                self->state.kind = ZombieStateChase;
            }
            break;
        }
        case ZombieStateChase: {
            Vector2 direction = {
                .x = player->shape.x - self->shape.x,
                .y = player->shape.y - self->shape.y,
            };
            direction = Vector2Normalize(direction);

            self->shape.x += direction.x * self->speed;
            self->shape.y += direction.y * self->speed;
            break;
        }
    }

    if (CheckCollisionRecs(self->shape, player->shape)) {
        player_get_hit(state);
    }
}

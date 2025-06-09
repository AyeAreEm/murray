#include "defs.h"

void zombie_draw(Zombie self) {
    DrawTextureEx(self.tex, (Vector2){self.shape.x, self.shape.y}, 0.0f, 2.0f, WHITE);
    // DrawTexture(self.tex, self.shape.x, self.shape.y, WHITE);
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
        printf("dead, x: %.2f, y: %.2f\n", player.shape.x, player.shape.y);
    }
}

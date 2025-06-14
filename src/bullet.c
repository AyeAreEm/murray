#include "defs.h"

void bullet_draw(Bullet self) {
    DrawRectangleRec(self.shape, GOLD);
}

bool bullet_update(Bullet *self, Player player) {
    self->shape.x += self->direction.x * BULLET_SPEED;
    self->shape.y += self->direction.y * BULLET_SPEED;

    if (
        player.shape.x + BULLET_DESPAWN_DIST <= self->shape.x ||
        player.shape.x - BULLET_DESPAWN_DIST >= self->shape.x ||
        player.shape.y + BULLET_DESPAWN_DIST <= self->shape.y || 
        player.shape.y - BULLET_DESPAWN_DIST >= self->shape.y
    ) {
        return true;
    }

    return false;
}

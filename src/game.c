#include "include/dyn.h"
#include "defs.h"
#include "raylib.h"

void game_draw(Game self) {
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
    EndDrawing();
}

void game_update(Game *self) {
    player_update(self);

    if (self->bullets != NULL) {
        for (size_t i = 0; i < dynlen(self->bullets); i++) {
            bool remove = bullet_update(&self->bullets[i], self->player);
            if (remove) {
                dynremove(Bullet, self->bullets, i);
            }

            for (size_t j = 0; j < dynlen(self->zombies); j++) {
                if (CheckCollisionRecs(self->bullets[i].shape, self->zombies[j].shape)) {
                    dynremove(Zombie, self->zombies, j);

                    self->bullets[i].health -= 1;
                    if (self->bullets[i].health == 0) {
                        dynremove(Bullet, self->bullets, i);
                    }
                }
            }
        }
    }

    if (self->zombies == NULL) {
        return;
    }

    // O(n^2) booooo
    for (size_t i = 0; i < dynlen(self->zombies); i++) {
        zombie_update(&self->zombies[i], self->player);

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

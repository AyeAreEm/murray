#include <stddef.h>
#include <raylib.h>
#include <raymath.h>

#define WIDTH 800.0
#define HEIGHT 600.0

#define ENTITY_SIZE 50.0

#define PLAYER_WALK 3.0
#define PLAYER_RUN 6.0

#define ZOMBIE_START_SPEED 2.0

typedef struct Player {
    Rectangle shape;
    float speed;
} Player;

typedef struct Zombie {
    Rectangle shape;
    float speed;
} Zombie;

typedef struct Game {
    Player player;
    Zombie zombies[100]; // TODO: dynamic array?
} Game;

void player_draw(Player self) {
    DrawRectangleRec(self.shape, GREEN);
}

void player_update(Player *self) {
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
}

void zombie_draw(Zombie self) {
    DrawRectangleRec(self.shape, RED);
}

void zombie_update(Zombie *self, Player player) {
    Vector2 direction = {
        .x = player.shape.x - self->shape.x,
        .y = player.shape.y - self->shape.y,
    };

    if (direction.x != 0 && direction.y != 0) {
        direction = Vector2Normalize(direction);
    }

    self->shape.x += direction.x * self->speed;
    self->shape.y += direction.y * self->speed;
}

void game_draw(Game self) {
    BeginDrawing();
    ClearBackground(BLACK);

    player_draw(self.player);
    zombie_draw(self.zombies[0]);

    EndDrawing();
}

void game_update(Game *self) {
    player_update(&self->player);

    zombie_update(&self->zombies[0], self->player);
}

int main() {
    InitWindow(WIDTH, HEIGHT, "Murray");
    SetTargetFPS(60);
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    Player player = {
        .shape = {
            .x = WIDTH/2,
            .y = HEIGHT/2,
            .width = ENTITY_SIZE,
            .height = ENTITY_SIZE,
        },
        .speed = PLAYER_WALK,
    };
    Game game = {
        .player = player,
        .zombies = {
            (Zombie){
                .shape = {
                    .x = WIDTH,
                    .y = HEIGHT,
                    .width = ENTITY_SIZE,
                    .height = ENTITY_SIZE,
                },
                .speed = ZOMBIE_START_SPEED,
            }
        }
    };

    while (!WindowShouldClose()) {
        game_draw(game);
        game_update(&game);
    }

    CloseWindow();
}

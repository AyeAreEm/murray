#ifndef MurrayDefs
#define MurrayDefs

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>
#include "include/dyn.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define WIDTH 1280.0f
#define HEIGHT 720.0f

#define ENTITY_SCALE 64.0f

#define ABS(x) ((x) < 0 ? -(x) : (x))

typedef struct Game Game;
/********** GUN **********/
typedef enum GunKind {
    GunPistol,
} GunKind;

typedef struct Gun {
    GunKind kind;
    const char* name;
    u8 fire_rate;
    u32 mag;
    u32 reserve;
} Gun;

/********** BULLET **********/
#define BULLET_WIDTH (ENTITY_SCALE / 6.4f)
#define BULLET_HEIGHT (ENTITY_SCALE / 6.4f)
#define BULLET_SPEED 2.0f
#define BULLET_DESPAWN_DIST (WIDTH * 2.0f)
typedef struct Player Player;

typedef struct Bullet {
    Rectangle shape;
    Vector2 direction;
} Bullet;

void bullet_draw(Bullet self);
// returns true if needs to be removed from bullets array
bool bullet_update(Bullet *self, Player player); 

/********** PLAYER **********/
#define PLAYER_WIDTH (ENTITY_SCALE / 2.15f)
#define PLAYER_HEIGHT (ENTITY_SCALE)
#define PLAYER_WALK 3.0f
#define PLAYER_RUN 6.0f


typedef struct Player {
    Rectangle shape;
    Texture tex;
    Camera2D camera;
    float speed;
    Gun gun;
} Player;

void player_draw(Player self);
void player_update(Game *game);
Bullet player_fire(Player *self);


/********** ZOMBIE **********/
#define ZOMBIE_WIDTH (ENTITY_SCALE / 2.15f)
#define ZOMBIE_HEIGHT (ENTITY_SCALE)
#define ZOMBIE_START_SPEED 2.0f

typedef struct Zombie {
    Rectangle shape;
    Texture tex;
    float speed;
} Zombie;

void zombie_draw(Zombie self);
void zombie_update(Zombie *self, Player player);

/********** GAME **********/
typedef struct Game {
    Player player;
    Zombie* zombies;
    Bullet* bullets;
} Game;

void game_draw(Game self);
void game_update(Game *self);

dyndefs(Zombie, Zombie);
dyndefs(Bullet, Bullet);
#endif // MurrayDefs

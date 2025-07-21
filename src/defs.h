#ifndef MURRAYDEFS_H
#define MURRAYDEFS_H

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>
#include <assert.h>
#include "include/exrand.h"

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__sun) || defined(__CYGWIN__)
#include <sys/types.h>
#elif defined(_WIN32) || defined(__MINGW32__)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif
typedef ssize_t isize;

/********** FORWARD DECLS **********/
typedef struct State State;
typedef struct Menu Menu;
typedef struct Game Game;
typedef struct Player Player;
typedef struct Zombie Zombie;
typedef struct Gun Gun;
typedef struct Bullet Bullet;

/********** GLOBALS **********/
extern bool debug_mode;
extern const Gun glock;
extern const Gun barrett;
extern const Gun rari;

#define WIDTH 1280.0f
#define HEIGHT 720.0f

#define BULLETS_LEN 255
#define ZOMBIES_LEN 1024

#define ENTITY_SCALE 64.0f

#define MELEE_DMG 5

#define MOUSE_X(player) ((player).shape.x + (GetMousePosition().x - (WIDTH / 2.0f)))
#define MOUSE_Y(player) ((player).shape.y + (GetMousePosition().y - (HEIGHT / 2.0f)))
#define MOUSE_XY(player) ((Vector2){MOUSE_X((player)), MOUSE_Y((player))})

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define PUSH(xs, len, tail_idx, item) (assert((tail_idx) < (len)), (xs)[(tail_idx)++] = (item))
#define REMOVE(xs, len, idx)\
    do {\
        assert((idx) < (len));\
        for (size_t remove_macro_index = (idx); remove_macro_index + 1 < (len); remove_macro_index++) {\
            xs[remove_macro_index] = xs[remove_macro_index + 1];\
        }\
        (len)--;\
    } while (0)
#define FILL(dest, dest_count, src, src_count, len)\
    do {\
        (dest_count) = 0;\
        for (size_t fill_macro_index = 0; fill_macro_index < (src_count); fill_macro_index++) {\
            PUSH((dest), (len), (dest_count), (src)[fill_macro_index]);\
        }\
    } while (0)\


/********** TEXTURES **********/
typedef enum TextureKind {
    TexturePlayer = 0,
    TextureZombie,
    TextureCount,
} TextureKind;

/********** GUN **********/
typedef enum GunKind {
    GunSemi,
    GunFull,
    GunBoltAction,
} GunKind;

typedef struct Gun {
    GunKind kind;
    const char* name;

    // stats
    uint8_t fire_rate;
    uint32_t max_mag;
    uint32_t max_reserve;

    uint16_t damage;

    uint32_t mag;
    uint32_t reserve;
} Gun;

/********** BULLET **********/
#define BULLET_WIDTH (ENTITY_SCALE / 6.4f)
#define BULLET_HEIGHT (ENTITY_SCALE / 6.4f)
#define BULLET_DESPAWN_DIST (WIDTH * 1.5f)
#define BULLET_SPEED 8.0f

typedef struct Bullet {
    Rectangle shape;
    Vector2 direction;
    uint16_t damage;
} Bullet;
extern Bullet bullets[BULLETS_LEN];

void bullet_draw(Bullet self);
// returns true if needs to be removed from bullets array
bool bullet_update(Bullet *self, Player player); 

/********** PLAYER **********/
#define PLAYER_WIDTH 32.0f
#define PLAYER_HEIGHT (ENTITY_SCALE)

#define PLAYER_MELEE_WIDTH 40.0f
#define PLAYER_MELEE_HEIGHT 40.0f

#define PLAYER_WALK 3.0f
#define PLAYER_RUN 6.0f

typedef struct Player {
    Camera2D camera;

    Rectangle shape;
    Texture tex;

    Rectangle melee_area;
    bool melee_area_active;

    float speed;
    Gun gun;

    uint8_t max_health;
    uint8_t health;
    double regen_time;

    bool in_iframe;
    double iframe_time;

    uint32_t points;
} Player;

Player player_init(Texture tex);
void player_draw(Player self);
void player_update(State *state);
void player_get_hit(State *state);
Bullet player_fire(Player *self);

/********** ZOMBIE **********/
#define ZOMBIE_WIDTH 32.0f
#define ZOMBIE_HEIGHT (ENTITY_SCALE)
#define ZOMBIE_START_SPEED 2.0f
#define ZOMBIE_MAX_SPEED 6.0f
#define ZOMBIE_BASE_HEALTH 5

typedef enum ZombieStateKind {
    ZombieStateSpawn,
    ZombieStateChase,
} ZombieStateKind;

typedef struct ZombieState {
    ZombieStateKind kind;
    union {
        double spawn_time;
    };
} ZombieState;

typedef struct Zombie {
    ZombieState state;

    Rectangle shape;
    Texture tex;
    float speed;

    uint32_t max_health;
    uint32_t health;
} Zombie;
extern Zombie zombies[ZOMBIES_LEN];

Zombie zombie_spawn(Game game, Texture tex);
void zombie_draw(Zombie self);
void zombie_update(Zombie *self, State *state);

/********** GAME **********/
typedef struct Game {
    Player player;

    Zombie *zombies;
    size_t zombies_count;

    float zombie_speed;
    size_t zombies_per_round;
    uint32_t zombie_health;

    Bullet *bullets;
    size_t bullets_count;

    Texture textures[TextureCount];

    uint16_t round;
    double round_transition;
    bool in_round_transition;
} Game;

Game game_init();
void draw_hud(Game game);
void game_draw(State state);
void game_update(State *state);

/********** STATE **********/
typedef enum StateKind {
    StateMenu,
    StatePlay,
    StateGameOver,
} StateKind;

typedef struct Button {
    Rectangle shape;
    const char *text;
} Button;

typedef struct Menu {
    Button play;
    Button quit;
    Texture title;
    Vector2 title_dimensions;
} Menu;

typedef struct GameOver {
} GameOver;

typedef struct State {
    StateKind kind;
    bool running;
    union {
        Menu menu;
        Game play;
        GameOver gameover;
    };
} State;

Menu menu_init();
void menu_draw(State state);
void menu_update(State *state);

void gameover_draw(State state);
void gameover_update(State *state);

#endif // MURRAYDEFS_H

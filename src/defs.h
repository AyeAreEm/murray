#ifndef MURRAYDEFS_H
#define MURRAYDEFS_H

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>
#include "include/dyn.h"
#include "include/exrand.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

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

#define ENTITY_SCALE 64.0f

#define MELEE_DMG 5

#define MOUSE_X(player) ((player).shape.x + (GetMousePosition().x - (WIDTH / 2.0f)))
#define MOUSE_Y(player) ((player).shape.y + (GetMousePosition().y - (HEIGHT / 2.0f)))
#define MOUSE_XY(player) ((Vector2){MOUSE_X((player)), MOUSE_Y((player))})

#define MIN(x, y) ((x) < (y) ? (x) : (y))

dyndefs(Zombie, Zombie);
dyndefs(Bullet, Bullet);
dyndefs(Texture, Texture);

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
    u8 fire_rate;
    u32 max_mag;
    u32 max_reserve;

    u16 damage;

    u32 mag;
    u32 reserve;
} Gun;

/********** BULLET **********/
#define BULLET_WIDTH (ENTITY_SCALE / 6.4f)
#define BULLET_HEIGHT (ENTITY_SCALE / 6.4f)
#define BULLET_DESPAWN_DIST (WIDTH * 2.0f)
#define BULLET_SPEED 8.0f

typedef struct Bullet {
    Rectangle shape;
    Vector2 direction;
    u16 damage;
} Bullet;

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

    u8 max_health;
    u8 health;
    double regen_time;

    bool in_iframe;
    double iframe_time;

    u32 points;
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

    u32 max_health;
    u32 health;
} Zombie;

Zombie zombie_spawn(Game game, Texture tex);
void zombie_draw(Zombie self);
void zombie_update(Zombie *self, State *state);

/********** GAME **********/
typedef struct Game {
    Player player;

    Zombie *zombies;
    float zombie_speed;
    usize zombies_per_round;
    u32 zombie_health;

    Bullet *bullets;

    Texture textures[TextureCount];

    u16 round;
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

#include "defs.h"

const Gun glock = {
    .kind = GunSemi,
    .name = "Glock",

    .fire_rate = 5,
    .max_mag = 24,
    .max_reserve = 240,

    .damage = 1,

    .mag = 24,
    .reserve = 240,
};

// gun for testing. TODO: remove this gun at some point
const Gun rari = {
    .kind = GunFull,
    .name = "Rari",

    .fire_rate = 5,
    .max_mag = 1000,
    .max_reserve = 10000,

    .damage = 5,

    .mag = 1000,
    .reserve = 10000,
};

const Gun barrett = {
    .kind = GunBoltAction,
    .name = "Barret M90",

    .fire_rate = 1,
    .max_mag = 7,
    .max_reserve = 105,

    .damage = 150,

    .mag = 7,
    .reserve = 105,
};

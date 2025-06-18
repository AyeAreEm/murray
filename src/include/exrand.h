#ifndef EXRAND_H
#define EXRAND_H

#include <stdlib.h>
#include <time.h>

#define exrand_random_seed() (srand(time(NULL)));
#define exrand_range(min, max) (rand() % ((max) - (min) + 1) + min)

#endif // EXRAND_H

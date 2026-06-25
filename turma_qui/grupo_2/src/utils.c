#include <stdlib.h>
#include <time.h>

#include "utils.h"

void seed_random(void) {
    srand((unsigned int)time(NULL));
}

int random_int(int min, int max) {
    if (max <= min) {
        return min;
    }

    return min + rand() % (max - min + 1);
}

int chance(int percent) {
    if (percent <= 0) {
        return 0;
    }

    if (percent >= 100) {
        return 1;
    }

    return random_int(1, 100) <= percent;
}

int positions_equal(Position a, Position b) {
    return a.row == b.row && a.col == b.col;
}

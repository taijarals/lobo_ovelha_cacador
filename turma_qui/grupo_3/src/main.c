#include "engine/engine.h"
#include "graph/window.h"
#include "graph/renderer.h"

#include <stdio.h>
#include <time.h>

int delta = 0;

int main(int argc, char const *argv[])
{
    printf("Hello, World!\n");

    if (!window_init()) {
        fprintf(stderr, "Not able to initialize the window");
        return 1;
    }

    printf("Initializing simulator\n");

    GameConfig config;
    config.map_width = 32;
    config.map_height = 32;
    config.sheep_distribution = 0.12f;
    config.wolf_distribution = 0.12f;
    config.hunter_distribution = 0.12f;
    config.seed = (uint64_t) time(NULL);;

    game_create_world(config);
    render_game_reset(config);

    while (!window_closed()) {
        if (delta++ % 10 == 0) game_update();
        render_frame();
    }

    window_deinit();

    return 0;
}

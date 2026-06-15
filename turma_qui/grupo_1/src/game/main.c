#include "core/core.h"
#include "graph/window.h"
#include "graph/renderer.h"

int main(int argc, char const *argv[])
{
    printf("Hello, World!");

    if (!window_init()) {
        fprintf(stderr, "Not able to initialize the window");
        return 1;
    }

    while (!window_closed()) render_frame();
    

    window_deinit();

    return 0;
}

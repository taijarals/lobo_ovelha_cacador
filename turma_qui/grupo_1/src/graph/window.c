#include "graph/window.h"

int window_init() {
    InitWindow(800, 600, "Predator-Prey Ecosystem Simulator");
    if (!IsWindowReady()) return false;
    
    SetTargetFPS(60);

    return true;
}
void window_deinit() {
    CloseWindow();
}

int window_closed() {
    return WindowShouldClose();
}

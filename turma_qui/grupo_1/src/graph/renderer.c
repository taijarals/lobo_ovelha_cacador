#include "graph/renderer.h"

void render_frame() {
    BeginDrawing();
    ClearBackground(BLUE);
    DrawText("Hello, world 😛", 10, 10, 20, BLACK);
    DrawText("Is this font monospaced?", 10, 30, 20, BLACK);
    EndDrawing();
}

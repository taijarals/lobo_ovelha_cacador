#ifndef BACKGROUND_H_
#define BACKGROUND_H_

#include <raylib.h>
#include "../include/core.h"

typedef struct {
  Texture2D texture;
  Rectangle dest_rect;
  Vector2 rotation;
} Background;

void draw_matrix(Background *lines);

#endif // !BACKGROUND_H_

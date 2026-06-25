#ifndef ENTITIE_H_
#define ENTITIE_H_

#include "../include/core.h"
#include <raylib.h>
#include <string.h>

typedef struct Sprite {
  Texture2D texture;
  Rectangle dest_rect;
  Rectangle sprite_animation;
  Vector2 rotation;
  float time_move;
  float move_interval;
  int gridx;
  int gridy;
  EntityType type;
} Sprite;

typedef struct {
  Sprite *entities;
  int list_size;
} EntityList;

void create_entities_2(Sprite *entitie, const char *file_path,
                       Rectangle dest_rect, Rectangle sprite_animation,
                       EntityType type);

void unload_texture(Sprite *entitie);

void draw_entitie(Sprite *entitie, int *animation_frame);
void set_grid_position(Sprite *entitie, int gridX, int gridY);

EntityList create_entity_list(EntityType type, int count);
void remove_entity(EntityList *list, int index);

#endif // !ENTITIE_H_

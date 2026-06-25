#include "entities.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void create_entities_2(Sprite *entitie, const char *file_path,
                       Rectangle dest_rect, Rectangle sprite_animation,
                       EntityType type) {

  entitie->rotation = (Vector2){0, 0};
  Texture2D tex = LoadTexture(file_path);

  if (tex.id == 0) {
    printf("Erro ao carregar textura: %s\n", file_path);
    return;
  }
  entitie->time_move = 0.0f;
  entitie->move_interval = 2.5f;
  entitie->texture = tex;
  entitie->dest_rect = dest_rect;
  entitie->sprite_animation = sprite_animation;
  entitie->type = type;
  entitie->gridx = 0;
  entitie->gridy = 0;
}

void unload_texture(Sprite *entitie) { UnloadTexture(entitie->texture); }

void draw_entitie(Sprite *entitie, int *animation_frame) {
  entitie->sprite_animation.x =
      entitie->sprite_animation.width * (*animation_frame);

  DrawTexturePro(entitie->texture, entitie->sprite_animation,
                 entitie->dest_rect, entitie->rotation, 0.0, RAYWHITE);
}

void set_grid_position(Sprite *entitie, int gridX, int gridY) {
  entitie->gridx = gridX;
  entitie->gridy = gridY;
  entitie->dest_rect.x = MATRIX_WIDTH_DIS + (gridX * CELL_SIZE) +
                         (CELL_SIZE - entitie->dest_rect.width) / 2.0f;
  entitie->dest_rect.y = MATRIX_HEIGHT_DIS + (gridY * CELL_SIZE) +
                         (CELL_SIZE - entitie->dest_rect.height) / 2.0f;
}

EntityList create_entity_list(EntityType type, int count) {
  EntityList list;
  list.list_size = count;
  list.entities = (Sprite *)malloc(sizeof(Sprite) * count);

  if (list.entities == NULL) {
    printf("Failed to allocate entity list\n");
    list.list_size = 0;
    return list;
  }

  const char *file_path;
  Rectangle dest_rect;
  Rectangle sprite_animation;

  switch (type) {
  case HUNTER:
    file_path = HUNTER_ASSETS;
    dest_rect =
        (Rectangle){0, 0, HUNTER_DEST_RECT_WIDTH, HUNTER_DEST_RECT_HEIGHT};
    sprite_animation =
        (Rectangle){HUNTER_IDLE_FRAME, 0, HUNTER_IDLE_FRAME_WIDTH,
                    HUNTER_IDLE_FRAME_HEIGHT};
    break;
  case VAMPIRE:
    file_path = VAMPIRE_ASSETS;
    dest_rect =
        (Rectangle){0, 0, VAMPIRE_DEST_RECT_WIDTH, VAMPIRE_DEST_RECT_HEIGHT};
    sprite_animation =
        (Rectangle){VAMPIRE_IDLE_FRAME, 0, VAMPIRE_IDLE_FRAME_WIDTH,
                    VAMPIRE_IDLE_FRAME_HEIGHT};
    break;
  case WEREWOLF:
    file_path = WEREWOLF_ASSETS;
    dest_rect =
        (Rectangle){0, 0, WEREWOLF_DEST_RECT_WIDTH, WEREWOLF_DEST_RECT_HEIGHT};
    sprite_animation =
        (Rectangle){WEREWOLF_IDLE_FRAME_X, WEREWOLF_IDLE_FRAME_Y,
                    WEREWOLF_IDLE_FRAME_WIDTH, WEREWOLF_IDLE_FRAME_HEIGHT};
    break;
  default:
    printf("Unknown entity type\n");
    free(list.entities);
    list.entities = NULL;
    list.list_size = 0;
    return list;
  }

  for (int i = 0; i < count; i++) {
    create_entities_2(&list.entities[i], file_path, dest_rect, sprite_animation,
                      type);
  }

  return list;
}

void remove_entity(EntityList *list, int index) {
  if (index < 0 || index >= list->list_size) {
    printf("Invalid index for remove_entity\n");
    return;
  }

  // Unload the texture of the dying entity
  UnloadTexture(list->entities[index].texture);

  // Shift elements down to fill the gap
  for (int i = index; i < list->list_size - 1; i++) {
    list->entities[i] = list->entities[i + 1];
  }

  list->list_size--;

  if (list->list_size == 0) {
    free(list->entities);
    list->entities = NULL;
    printf("Entity list is now empty\n");
    return;
  }

  Sprite *temp =
      (Sprite *)realloc(list->entities, sizeof(Sprite) * list->list_size);
  if (temp == NULL) {
    printf("realloc failed in remove_entity\n");
    return;
  }
  list->entities = temp;
}

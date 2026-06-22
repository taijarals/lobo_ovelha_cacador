#include "entities/entities.h"
#include "include/core.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

float minInterval = 0.1f;
float acceleration = 0.02f;

float timer_idle = 0.0f;
float idle_interval = 0.12f;

int game_over = 0;
const char *winner_name = NULL;

void update_idle(int *idle_animation);
void give_init_position(Sprite *entity, EntityList **lists, int list_count);
int is_cell_occupied(int gridX, int gridY, EntityList **lists, int list_count);
void move_entity(Sprite *entity, EntityList **lists, int list_count);
int check_combat(EntityType attacker, EntityType defender);
void draw_grid(Texture2D lineTexture);
void check_winner(EntityList **lists, int list_count);
void draw_winner_banner(void);

int main(int argc, char *argv[]) {
  ChangeDirectory(GetApplicationDirectory());
  int idle_animation = 0;

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Main");

  EntityList hunter_list = create_entity_list(HUNTER, 3);
  EntityList vampire_list = create_entity_list(VAMPIRE, 3);
  EntityList werewolf_list = create_entity_list(WEREWOLF, 3);

  EntityList *all_lists[3] = {&hunter_list, &vampire_list, &werewolf_list};

  for (int i = 0; i < hunter_list.list_size; i++)
    give_init_position(&hunter_list.entities[i], all_lists, 3);
  for (int i = 0; i < vampire_list.list_size; i++)
    give_init_position(&vampire_list.entities[i], all_lists, 3);
  for (int i = 0; i < werewolf_list.list_size; i++)
    give_init_position(&werewolf_list.entities[i], all_lists, 3);

  Texture2D lineTexture = LoadTexture(BACKGROUND_LINE_ASSEST);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    if (!game_over) {
      float delta = GetFrameTime();
      timer_idle += delta;

      for (int i = 0; i < hunter_list.list_size; i++)
        hunter_list.entities[i].time_move += delta;
      for (int i = 0; i < vampire_list.list_size; i++)
        vampire_list.entities[i].time_move += delta;
      for (int i = 0; i < werewolf_list.list_size; i++)
        werewolf_list.entities[i].time_move += delta;

      for (int i = hunter_list.list_size - 1; i >= 0; i--)
        move_entity(&hunter_list.entities[i], all_lists, 3);
      for (int i = vampire_list.list_size - 1; i >= 0; i--)
        move_entity(&vampire_list.entities[i], all_lists, 3);
      for (int i = werewolf_list.list_size - 1; i >= 0; i--)
        move_entity(&werewolf_list.entities[i], all_lists, 3);

      check_winner(all_lists, 3);
    }

    BeginDrawing();
    ClearBackground((Color){0, 117, 44, 255});

    draw_grid(lineTexture);

    for (int i = 0; i < hunter_list.list_size; i++)
      draw_entitie(&hunter_list.entities[i], &idle_animation);
    for (int i = 0; i < vampire_list.list_size; i++)
      draw_entitie(&vampire_list.entities[i], &idle_animation);
    for (int i = 0; i < werewolf_list.list_size; i++)
      draw_entitie(&werewolf_list.entities[i], &idle_animation);

    update_idle(&idle_animation);

    if (game_over)
      draw_winner_banner();

    EndDrawing();
  }

  for (int i = 0; i < hunter_list.list_size; i++)
    unload_texture(&hunter_list.entities[i]);
  for (int i = 0; i < vampire_list.list_size; i++)
    unload_texture(&vampire_list.entities[i]);
  for (int i = 0; i < werewolf_list.list_size; i++)
    unload_texture(&werewolf_list.entities[i]);

  free(hunter_list.entities);
  free(vampire_list.entities);
  free(werewolf_list.entities);

  UnloadTexture(lineTexture);
  CloseWindow();

  return EXIT_SUCCESS;
}

// --------------------------
// Functions
// --------------------------

void draw_grid(Texture2D lineTexture) {
  for (int i = 0; i <= MATRIX_NUMBER_ROWS; i++) {
    DrawTexturePro(lineTexture, (Rectangle){0, 0, 2, 12},
                   (Rectangle){MATRIX_WIDTH_DIS,
                               MATRIX_HEIGHT_DIS + i * CELL_SIZE, 480, 2},
                   (Vector2){0, 0}, 0.0f, (Color){255, 255, 255, 128});
  }
  for (int i = 0; i <= MATRIX_NUMBER_COLUMNS; i++) {
    DrawTexturePro(lineTexture, (Rectangle){0, 0, 2, 12},
                   (Rectangle){MATRIX_WIDTH_DIS + i * CELL_SIZE,
                               MATRIX_HEIGHT_DIS, 2, 480},
                   (Vector2){0, 0}, 0.0f, (Color){255, 255, 255, 128});
  }
}

int is_cell_occupied(int gridX, int gridY, EntityList **lists, int list_count) {
  for (int l = 0; l < list_count; l++) {
    for (int i = 0; i < lists[l]->list_size; i++) {
      if (lists[l]->entities[i].gridx == gridX &&
          lists[l]->entities[i].gridy == gridY) {
        return 1;
      }
    }
  }
  return 0;
}

// Returns 1 if the mover kills the target, 0 if mover dies, -1 if no combat
int check_combat(EntityType attacker, EntityType defender) {
  if (attacker == VAMPIRE && defender == HUNTER)
    return 1;
  if (attacker == HUNTER && defender == WEREWOLF)
    return 1;
  if (attacker == WEREWOLF && defender == VAMPIRE)
    return 1;
  if (attacker == HUNTER && defender == VAMPIRE)
    return 0;
  if (attacker == WEREWOLF && defender == HUNTER)
    return 0;
  if (attacker == VAMPIRE && defender == WEREWOLF)
    return 0;
  return -1; // same type
}

void move_entity(Sprite *entity, EntityList **lists, int list_count) {
  if (entity->time_move >= entity->move_interval) {
    entity->time_move = 0.0f;

    int newX = entity->gridx;
    int newY = entity->gridy;

    switch (GetRandomValue(0, 3)) {
    case UP:
      newY -= 1;
      break;
    case DOWN:
      newY += 1;
      break;
    case LEFT:
      newX -= 1;
      break;
    case RIGHT:
      newX += 1;
      break;
    }

    // Boundary check
    if (newX < 0 || newX >= MATRIX_NUMBER_COLUMNS || newY < 0 ||
        newY >= MATRIX_NUMBER_ROWS) {
      printf("Boundary hit, movement lost\n");
      return;
    }

    // Check all lists for occupancy
    for (int l = 0; l < list_count; l++) {
      for (int i = 0; i < lists[l]->list_size; i++) {
        Sprite *target = &lists[l]->entities[i];

        if (target == entity)
          continue; // skip self

        if (target->gridx == newX && target->gridy == newY) {
          int result = check_combat(entity->type, target->type);

          if (result == 1) {
            // Attacker wins, remove defender
            printf("Entity type %d killed entity type %d at (%d, %d)\n",
                   entity->type, target->type, newX, newY);
            remove_entity(lists[l], i);
            // Mover proceeds to the cell
            set_grid_position(entity, newX, newY);
          } else if (result == 0) {
            // Attacker loses, remove self
            printf("Entity type %d was killed by entity type %d at (%d, %d)\n",
                   entity->type, target->type, newX, newY);
            // Find which list the mover belongs to and remove it
            for (int ml = 0; ml < list_count; ml++) {
              for (int mi = 0; mi < lists[ml]->list_size; mi++) {
                if (&lists[ml]->entities[mi] == entity) {
                  remove_entity(lists[ml], mi);
                  return;
                }
              }
            }
          } else {
            // Same type, no combat, movement blocked
            printf("Same type collision, movement blocked\n");
          }
          return;
        }
      }
    }

    // No collision, move freely
    set_grid_position(entity, newX, newY);
    printf("Entity moved to grid (%d, %d)\n", entity->gridx, entity->gridy);

    if (entity->move_interval > minInterval) {
      entity->move_interval *= (1.0f - acceleration);
      if (entity->move_interval < minInterval)
        entity->move_interval = minInterval;
    }
  }
}

void update_idle(int *idle_animation) {
  if (timer_idle >= idle_interval) {
    timer_idle = 0.0f;
    *idle_animation += 1;
    if (*idle_animation > 4)
      *idle_animation = 0;
  }
}

void give_init_position(Sprite *entity, EntityList **lists, int list_count) {
  int gridX, gridY;
  int placed = 0;

  while (!placed) {
    // Pick a random starting cell
    gridX = GetRandomValue(0, MATRIX_NUMBER_COLUMNS - 1);
    gridY = GetRandomValue(0, MATRIX_NUMBER_ROWS - 1);

    if (!is_cell_occupied(gridX, gridY, lists, list_count)) {
      placed = 1;
      break;
    }

    // Try neighbors: right -> down -> left -> up
    int neighbors[4][2] = {{gridX + 1, gridY},
                           {gridX, gridY + 1},
                           {gridX - 1, gridY},
                           {gridX, gridY - 1}};

    for (int n = 0; n < 4; n++) {
      int nx = neighbors[n][0];
      int ny = neighbors[n][1];

      // Skip out of bounds
      if (nx < 0 || nx >= MATRIX_NUMBER_COLUMNS || ny < 0 ||
          ny >= MATRIX_NUMBER_ROWS)
        continue;

      if (!is_cell_occupied(nx, ny, lists, list_count)) {
        gridX = nx;
        gridY = ny;
        placed = 1;
        break;
      }
    }
    // If placed is still 0, the while loop picks a new random cell
  }

  set_grid_position(entity, gridX, gridY);
  printf("Init position: grid (%d, %d)\n", gridX, gridY);
}

void check_winner(EntityList **lists, int list_count) {
  if (game_over)
    return;

  int alive_count = 0;
  int last_alive = -1;

  for (int l = 0; l < list_count; l++) {
    if (lists[l]->list_size > 0) {
      alive_count++;
      last_alive = l;
    }
  }

  if (alive_count == 1) {
    game_over = 1;
    switch (lists[last_alive]->entities[0].type) {
    case HUNTER:
      winner_name = "Hunter";
      break;
    case VAMPIRE:
      winner_name = "Vampire";
      break;
    case WEREWOLF:
      winner_name = "Werewolf";
      break;
    default:
      winner_name = "Unknown";
      break;
    }
    printf("Winner: %s\n", winner_name);
  }
}

void draw_winner_banner(void) {
  // Semi-transparent dark box
  int boxWidth = 500;
  int boxHeight = 120;
  int boxX = (WINDOW_WIDTH - boxWidth) / 2;
  int boxY = (WINDOW_HEIGHT - boxHeight) / 2;

  DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){0, 0, 0, 180});

  // Winner text
  char message[64];
  snprintf(message, sizeof(message), "Winner: %s!", winner_name);

  int fontSize = 48;
  int textWidth = MeasureText(message, fontSize);
  int textX = (WINDOW_WIDTH - textWidth) / 2;
  int textY = boxY + (boxHeight - fontSize) / 2;

  DrawText(message, textX, textY, fontSize, WHITE);
}

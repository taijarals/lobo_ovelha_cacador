#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include "entity.h"

/*
 * Celula do tabuleiro:
 *   entity   -> ponteiro pra entidade que ocupa a celula (ou NULL)
 *   obstacle -> tipo de obstaculo fixo (ou OBSTACLE_NONE)
 */
typedef struct {
    Entity*      entity;
    ObstacleType obstacle;
} Cell;

/*
 * Tabuleiro alocado dinamicamente como array de arrays (grid[linha][coluna])
 * acesso direto grid[r][c] e O(1)
 */
typedef struct {
    int    rows;
    int    cols;
    Cell** grid;
} Board;

/* ciclo de vida */
Board* board_create (int rows, int cols);
void   board_destroy(Board* board);

/* consultas */
int board_is_valid  (const Board* board, int row, int col);
int board_is_blocked(const Board* board, int row, int col);
int board_is_empty  (const Board* board, int row, int col);

/* manipulacao */
void board_place_entity  (Board* board, Entity* e);
void board_remove_entity (Board* board, Entity* e);
void board_move_entity   (Board* board, Entity* e, int new_row, int new_col);
void board_place_obstacle(Board* board, int row, int col, ObstacleType type);

/* popula o mapa com obstaculos e entidades (Fisher-Yates) */
void board_populate(Board* board, EntityList* list);

#endif

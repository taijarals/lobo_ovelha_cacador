#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "entities.h"
#include "utils.h"

static char obstacle_symbol(ObstacleType type) {
    switch (type) {
        case OBSTACLE_TREE:
            return 'A';
        case OBSTACLE_ROCK:
            return 'P';
        default:
            return '.';
    }
}

static const Entity *find_entity_at(const EntityList *entities, Position pos) {
    for (int i = 0; i < entities->count; i++) {
        if (entities->items[i].alive && positions_equal(entities->items[i].pos, pos)) {
            return &entities->items[i];
        }
    }

    return NULL;
}

int board_init(Board *board, int size) {
    if (size < 5) {
        return 0;
    }

    board->size = size;
    board->obstacles = calloc((size_t)(size * size), sizeof(ObstacleType));

    return board->obstacles != NULL;
}

void board_free(Board *board) {
    free(board->obstacles);
    board->obstacles = NULL;
    board->size = 0;
}

int board_index(const Board *board, Position pos) {
    return pos.row * board->size + pos.col;
}

int board_in_bounds(const Board *board, Position pos) {
    return pos.row >= 0 && pos.row < board->size && pos.col >= 0 && pos.col < board->size;
}

int board_has_obstacle(const Board *board, Position pos) {
    return board_in_bounds(board, pos) && board->obstacles[board_index(board, pos)] != OBSTACLE_NONE;
}

void board_set_obstacle(Board *board, Position pos, ObstacleType type) {
    if (board_in_bounds(board, pos)) {
        board->obstacles[board_index(board, pos)] = type;
    }
}

ObstacleType board_get_obstacle(const Board *board, Position pos) {
    if (!board_in_bounds(board, pos)) {
        return OBSTACLE_NONE;
    }

    return board->obstacles[board_index(board, pos)];
}

Position board_random_position(const Board *board) {
    Position pos;
    pos.row = random_int(0, board->size - 1);
    pos.col = random_int(0, board->size - 1);
    return pos;
}

void board_print(const Board *board, const EntityList *entities, int round) {
    printf("\nRodada %d\n", round);
    printf("Legenda: H=humano, Z=zumbi, S=soldado, A=arvore, P=pedra, .=vazio\n\n");

    for (int row = 0; row < board->size; row++) {
        for (int col = 0; col < board->size; col++) {
            Position pos = { row, col };
            const Entity *entity = find_entity_at(entities, pos);

            if (entity != NULL) {
                printf("%c ", entity_symbol(entity->type));
            } else {
                printf("%c ", obstacle_symbol(board_get_obstacle(board, pos)));
            }
        }

        printf("\n");
    }
}

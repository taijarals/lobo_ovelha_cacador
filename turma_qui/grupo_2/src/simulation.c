#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "entities.h"
#include "simulation.h"
#include "utils.h"

#define HUMAN_PERCENT 12
#define ZOMBIE_PERCENT 12
#define SOLDIER_PERCENT 5
#define OBSTACLE_PERCENT 10
#define REPRODUCTION_CHANCE 18

static int percent_of_cells(int cells, int percent) {
    int value = (cells * percent) / 100;
    return value < 1 ? 1 : value;
}

static Entity *entity_at(EntityList *entities, Position pos) {
    for (int i = 0; i < entities->count; i++) {
        if (entities->items[i].alive && positions_equal(entities->items[i].pos, pos)) {
            return &entities->items[i];
        }
    }

    return NULL;
}

static int occupied_by_entity(const EntityList *entities, Position pos) {
    for (int i = 0; i < entities->count; i++) {
        if (entities->items[i].alive && positions_equal(entities->items[i].pos, pos)) {
            return 1;
        }
    }

    return 0;
}

static int cell_is_free(const Board *board, const EntityList *entities, Position pos) {
    return board_in_bounds(board, pos)
        && !board_has_obstacle(board, pos)
        && !occupied_by_entity(entities, pos);
}

static Position random_free_position(const Board *board, const EntityList *entities) {
    Position pos;

    do {
        pos = board_random_position(board);
    } while (!cell_is_free(board, entities, pos));

    return pos;
}

static void populate_obstacles(Simulation *sim, int amount) {
    for (int i = 0; i < amount; i++) {
        Position pos = random_free_position(&sim->board, &sim->entities);
        ObstacleType type = chance(50) ? OBSTACLE_TREE : OBSTACLE_ROCK;
        board_set_obstacle(&sim->board, pos, type);
    }
}

static void populate_entities(Simulation *sim, EntityType type, int amount) {
    for (int i = 0; i < amount; i++) {
        Position pos = random_free_position(&sim->board, &sim->entities);
        entity_list_add(&sim->entities, type, pos);
    }
}

static Position random_neighbor(Position pos) {
    int direction = random_int(0, 3);

    if (direction == 0) {
        pos.row--;
    } else if (direction == 1) {
        pos.row++;
    } else if (direction == 2) {
        pos.col--;
    } else {
        pos.col++;
    }

    return pos;
}

static int can_eliminate(EntityType attacker, EntityType target) {
    return (attacker == ENTITY_ZOMBIE && target == ENTITY_HUMAN)
        || (attacker == ENTITY_SOLDIER && target == ENTITY_ZOMBIE);
}

static int can_share_cell(EntityType attacker, EntityType target) {
    return attacker == target;
}

static int find_empty_neighbor(const Board *board, const EntityList *entities, Position origin, Position *result) {
    Position options[4] = {
        { origin.row - 1, origin.col },
        { origin.row + 1, origin.col },
        { origin.row, origin.col - 1 },
        { origin.row, origin.col + 1 }
    };

    for (int attempts = 0; attempts < 4; attempts++) {
        int index = random_int(0, 3);
        Position candidate = options[index];

        if (cell_is_free(board, entities, candidate)) {
            *result = candidate;
            return 1;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (cell_is_free(board, entities, options[i])) {
            *result = options[i];
            return 1;
        }
    }

    return 0;
}

static void try_reproduce(Simulation *sim, Entity *entity, Entity *other) {
    Position child_pos;

    if (!can_share_cell(entity->type, other->type)) {
        return;
    }

    if (entity->age < 2 || other->age < 2 || !chance(REPRODUCTION_CHANCE)) {
        return;
    }

    if (find_empty_neighbor(&sim->board, &sim->entities, entity->pos, &child_pos)) {
        entity_list_add(&sim->entities, entity->type, child_pos);
    }
}

static void apply_energy(Entity *entity) {
    if (entity->type == ENTITY_ZOMBIE) {
        entity->energy--;

        if (entity->energy <= 0) {
            entity->alive = 0;
        }
    }
}

static void move_entity(Simulation *sim, int entity_index) {
    Entity *entity = &sim->entities.items[entity_index];

    if (!entity->alive) {
        return;
    }

    entity->age++;
    apply_energy(entity);

    if (!entity->alive) {
        return;
    }

    Position target_pos = random_neighbor(entity->pos);

    if (!board_in_bounds(&sim->board, target_pos) || board_has_obstacle(&sim->board, target_pos)) {
        return;
    }

    Entity *target = entity_at(&sim->entities, target_pos);

    if (target == NULL) {
        entity->pos = target_pos;
        return;
    }

    if (can_eliminate(entity->type, target->type)) {
        target->alive = 0;
        entity->pos = target_pos;

        if (entity->type == ENTITY_ZOMBIE) {
            entity->energy += 4;
        }

        return;
    }

    try_reproduce(sim, entity, target);
}

static int has_winner(const Simulation *sim) {
    Stats stats = simulation_stats(sim);

    if (stats.humans == 0) {
        printf("\nCondicao de parada: os humanos acabaram.\n");
        return 1;
    }

    if (stats.zombies == 0) {
        printf("\nCondicao de parada: os zumbis acabaram.\n");
        return 1;
    }

    return 0;
}

static void print_stats(const Simulation *sim) {
    Stats stats = simulation_stats(sim);

    printf("\nResumo: %d humanos | %d zumbis | %d soldados | %d obstaculos\n",
        stats.humans,
        stats.zombies,
        stats.soldiers,
        stats.obstacles);
}

int simulation_init(Simulation *sim, int size, int max_rounds) {
    int cells = size * size;
    int obstacle_count = percent_of_cells(cells, OBSTACLE_PERCENT);
    int human_count = percent_of_cells(cells, HUMAN_PERCENT);
    int zombie_count = percent_of_cells(cells, ZOMBIE_PERCENT);
    int soldier_count = percent_of_cells(cells, SOLDIER_PERCENT);

    sim->max_rounds = max_rounds;
    sim->current_round = 0;

    entity_list_init(&sim->entities);

    if (!board_init(&sim->board, size)) {
        entity_list_free(&sim->entities);
        return 0;
    }

    populate_obstacles(sim, obstacle_count);
    populate_entities(sim, ENTITY_HUMAN, human_count);
    populate_entities(sim, ENTITY_ZOMBIE, zombie_count);
    populate_entities(sim, ENTITY_SOLDIER, soldier_count);

    return 1;
}

void simulation_free(Simulation *sim) {
    board_free(&sim->board);
    entity_list_free(&sim->entities);
}

void simulation_run(Simulation *sim) {
    board_print(&sim->board, &sim->entities, sim->current_round);
    print_stats(sim);

    while (sim->current_round < sim->max_rounds && !has_winner(sim)) {
        sim->current_round++;
        int initial_count = sim->entities.count;

        for (int i = 0; i < initial_count; i++) {
            move_entity(sim, i);
        }

        entity_list_remove_dead(&sim->entities);
        board_print(&sim->board, &sim->entities, sim->current_round);
        print_stats(sim);
    }

    printf("\nSimulacao encerrada apos %d rodada(s).\n", sim->current_round);
}

Stats simulation_stats(const Simulation *sim) {
    Stats stats = { 0, 0, 0, 0 };
    int cells = sim->board.size * sim->board.size;

    for (int i = 0; i < sim->entities.count; i++) {
        if (!sim->entities.items[i].alive) {
            continue;
        }

        if (sim->entities.items[i].type == ENTITY_HUMAN) {
            stats.humans++;
        } else if (sim->entities.items[i].type == ENTITY_ZOMBIE) {
            stats.zombies++;
        } else if (sim->entities.items[i].type == ENTITY_SOLDIER) {
            stats.soldiers++;
        }
    }

    for (int i = 0; i < cells; i++) {
        if (sim->board.obstacles[i] != OBSTACLE_NONE) {
            stats.obstacles++;
        }
    }

    return stats;
}

#ifndef SIMULATION_H
#define SIMULATION_H

#include "board.h"
#include "entity.h"

/* contexto completo de uma execucao da simulacao */
typedef struct {
    Board*     board;
    EntityList list;
    int        round;
    int        max_rounds;
} Simulation;

Simulation* simulation_create (int rows, int cols, int max_rounds);
void        simulation_destroy(Simulation* sim);

int  simulation_check_victory(const Simulation* sim);
void simulation_round         (Simulation* sim);
void simulation_run           (Simulation* sim);

#endif

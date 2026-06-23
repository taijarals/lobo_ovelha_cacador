#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"
#include "movement.h"
#include "interaction.h"
#include "display.h"

/* cria e inicializa o contexto da simulacao */
Simulation* simulation_create(int rows, int cols, int max_rounds) {
    Simulation* sim = (Simulation*)malloc(sizeof(Simulation));
    if (!sim) {
        fprintf(stderr, "Erro: falha ao alocar Simulation.\n");
        return NULL;
    }

    sim->board = board_create(rows, cols);
    if (!sim->board) {
        free(sim);
        return NULL;
    }

    entity_list_init(&sim->list);
    sim->round      = 0;
    sim->max_rounds = max_rounds;

    board_populate(sim->board, &sim->list);

    return sim;
}

/* libera tudo na ordem certa: lista -> tabuleiro -> struct */
void simulation_destroy(Simulation* sim) {
    if (!sim) return;
    entity_list_destroy(&sim->list);
    board_destroy(sim->board);
    free(sim);
}

/*
 * Verifica se so restou uma faccao (ou nenhuma)
 * os contadores wolves/sheep/hunters ficam atualizados a cada
 * insercao/remocao, entao a verificacao e O(1) sem percorrer a lista
 */
int simulation_check_victory(const Simulation* sim) {
    int ativas = 0;
    if (sim->list.wolves  > 0) ativas++;
    if (sim->list.sheep   > 0) ativas++;
    if (sim->list.hunters > 0) ativas++;
    return (ativas <= 1);
}

/*
 * Executa uma rodada completa em 3 fases:
 *   1. Envelhecimento (incrementa rounds_alive)
 *   2. Movimento (cada entidade tenta se mover)
 *   3. Interacao (combate e reproducao)
 */
void simulation_round(Simulation* sim) {
    sim->round++;

    /* fase 1: todas as entidades vivas envelhecem */
    Entity* e = sim->list.head;
    while (e) {
        if (e->alive) e->rounds_alive++;
        e = e->next;
    }

    /* fase 2: movimento */
    movement_move_all(sim->board, &sim->list);

    /* fase 3: interacoes */
    interaction_process_all(sim->board, &sim->list);
}

/* loop principal da simulacao */
void simulation_run(Simulation* sim) {
    printf("\n=== ECOSSISTEMA INICIADO ===\n");
    printf("Mapa: %d x %d | Rodadas: %d\n\n",
           sim->board->rows, sim->board->cols, sim->max_rounds);

    display_legend();
    display_board(sim->board);
    display_stats(&sim->list, sim->round);

    while (sim->round < sim->max_rounds) {
        simulation_round(sim);
        display_board(sim->board);
        display_stats(&sim->list, sim->round);

        if (simulation_check_victory(sim)) {
            display_victory(&sim->list, sim->round);
            return;
        }
    }

    printf("\n=== LIMITE DE RODADAS ATINGIDO ===\n");
    display_final_stats(&sim->list, sim->round);
}

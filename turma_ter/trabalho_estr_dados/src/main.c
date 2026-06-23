#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "simulation.h"
#include "display.h"

int main(void) {
    /* semente aleatoria baseada no tempo pra cada execucao ser diferente */
    srand((unsigned int)time(NULL));

    printf("============================================\n");
    printf("   SIMULADOR DE ECOSSISTEMA - TRAB. ED     \n");
    printf("============================================\n\n");

    int rows, cols, rounds;

    /* pede as dimensoes do mapa - rejeita valores menores que o minimo */
    do {
        printf("Numero de linhas  (minimo %d): ", MIN_BOARD_SIZE);
        if (scanf("%d", &rows) != 1) rows = 0;
    } while (rows < MIN_BOARD_SIZE);

    do {
        printf("Numero de colunas (minimo %d): ", MIN_BOARD_SIZE);
        if (scanf("%d", &cols) != 1) cols = 0;
    } while (cols < MIN_BOARD_SIZE);

    do {
        printf("Numero de rodadas (minimo 1) : ");
        if (scanf("%d", &rounds) != 1) rounds = 0;
    } while (rounds < 1);

    printf("\n");

    Simulation* sim = simulation_create(rows, cols, rounds);
    if (!sim) {
        fprintf(stderr, "Erro: nao foi possivel criar a simulacao.\n");
        return EXIT_FAILURE;
    }

    simulation_run(sim);
    simulation_destroy(sim);

    return EXIT_SUCCESS;
}

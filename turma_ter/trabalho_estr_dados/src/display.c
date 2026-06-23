#include <stdio.h>
#include "display.h"

/* renderiza o tabuleiro em ASCII no terminal */
void display_board(const Board* board) {
    /* borda superior */
    printf("+");
    for (int c = 0; c < board->cols; c++) printf("--");
    printf("-+\n");

    for (int r = 0; r < board->rows; r++) {
        printf("| ");
        for (int c = 0; c < board->cols; c++) {
            const Cell* cell = &board->grid[r][c];

            if (cell->obstacle != OBSTACLE_NONE) {
                printf("%c ", (cell->obstacle == OBSTACLE_TREE) ? 'T' : 'R');
            } else if (cell->entity != NULL) {
                printf("%c ", entity_symbol(cell->entity->faction));
            } else {
                printf(". ");
            }
        }
        printf("|\n");
    }

    /* borda inferior */
    printf("+");
    for (int c = 0; c < board->cols; c++) printf("--");
    printf("-+\n");
}

/* mostra as estatisticas da rodada atual - O(1) por causa dos contadores */
void display_stats(const EntityList* list, int round) {
    printf("Rodada: %4d | Lobos: %4d | Ovelhas: %4d | Cacadores: %4d | Total: %4d\n",
           round, list->wolves, list->sheep, list->hunters, list->total);
}

void display_legend(void) {
    printf("LEGENDA:\n");
    printf("  W = Lobo     (predador - mata ovelha)\n");
    printf("  O = Ovelha   (presa)\n");
    printf("  C = Cacador  (caca o lobo, protege a ovelha indiretamente)\n");
    printf("  T = Arvore   (obstaculo)\n");
    printf("  R = Pedra    (obstaculo)\n");
    printf("  . = vazio\n\n");
}

void display_victory(const EntityList* list, int round) {
    const char* vencedor;

    if      (list->wolves  > 0) vencedor = "LOBOS";
    else if (list->sheep   > 0) vencedor = "OVELHAS";
    else if (list->hunters > 0) vencedor = "CACADORES";
    else                          vencedor = "NENHUM (extincao total)";

    printf("\n+=====================================+\n");
    printf("|       VENCEDOR DO ECOSSISTEMA       |\n");
    printf("+=====================================+\n");
    printf("|  %s\n", vencedor);
    printf("|  Rodada de vitoria: %d\n", round);
    printf("+=====================================+\n");
}

void display_final_stats(const EntityList* list, int round) {
    printf("\n=== RESULTADO FINAL (apos %d rodadas) ===\n", round);
    printf("  Lobos    : %d\n", list->wolves);
    printf("  Ovelhas  : %d\n", list->sheep);
    printf("  Cacadores: %d\n", list->hunters);
    printf("  Total    : %d entidades vivas\n", list->total);
    printf("==========================================\n");
}

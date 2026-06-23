#include <stdlib.h>
#include "movement.h"

/* deltas de linha e coluna pra cada direcao (cima/baixo/esq/dir) */
static const int DR[DIR_COUNT] = { -1,  1,  0,  0 };
static const int DC[DIR_COUNT] = {  0,  0, -1,  1 };

/*
 * Tenta mover a entidade pra uma celula vazia adjacente
 * embaralho as 4 direcoes antes de tentar pra nao criar vies
 * (sem isso toda entidade "prefere" subir, o que e bizarro)
 * retorna 1 se conseguiu mover, 0 se esta bloqueada
 */
int movement_move_entity(Board* board, Entity* e) {
    if (!e || !e->alive) return 0;

    /* cria e embaralha as direcoes localmente */
    int dirs[DIR_COUNT] = { 0, 1, 2, 3 };
    for (int i = DIR_COUNT - 1; i > 0; i--) {
        int j   = rand() % (i + 1);
        int tmp = dirs[i];
        dirs[i] = dirs[j];
        dirs[j] = tmp;
    }

    for (int i = 0; i < DIR_COUNT; i++) {
        int nr = e->row + DR[dirs[i]];
        int nc = e->col + DC[dirs[i]];

        if (board_is_empty(board, nr, nc)) {
            board_move_entity(board, e, nr, nc);
            return 1;
        }
    }

    return 0; /* bloqueada em todas as direcoes */
}

/*
 * Move todas as entidades da lista
 * o snapshot e necessario porque a reproducao pode adicionar
 * entidades novas ao final da lista durante a interacao
 * sem ele, entidades que nascem nessa rodada seriam movidas
 * antes mesmo de interagir, o que nao faz sentido
 */
void movement_move_all(Board* board, EntityList* list) {
    int count = list->total;
    if (count == 0) return;

    /* snapshot dos ponteiros existentes nessa rodada */
    Entity** snap = (Entity**)malloc(count * sizeof(Entity*));
    if (!snap) return;

    Entity* e = list->head;
    int     i = 0;
    while (e && i < count) {
        snap[i++] = e;
        e = e->next;
    }

    for (int k = 0; k < i; k++) {
        if (snap[k]->alive)
            movement_move_entity(board, snap[k]);
    }

    free(snap);
}

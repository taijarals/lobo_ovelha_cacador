#include <stdlib.h>
#include "interaction.h"

static const int DR[4] = { -1,  1,  0,  0 };
static const int DC[4] = {  0,  0, -1,  1 };

/* tenta criar um filho em celula vazia perto de e1 */
int interaction_reproduce(Board* board, EntityList* list,
                          const Entity* e1, const Entity* e2) {
    (void)e2; /* nao usamos e2 por enquanto */

    if (list->total >= MAX_ENTITIES) return 0;

    for (int d = 0; d < 4; d++) {
        int nr = e1->row + DR[d];
        int nc = e1->col + DC[d];

        if (board_is_empty(board, nr, nc)) {
            Entity* filho = entity_create(list, e1->faction, nr, nc);
            if (filho) {
                board_place_entity(board, filho);
                return 1;
            }
        }
    }
    return 0;
}

/*
 * Processa todas as interacoes da rodada
 *
 * Cadeia alimentar: Cacador -> Lobo -> Ovelha
 *   1. Predacao:    lobo adjacente a ovelha     -> mata a ovelha diretamente
 *   2. Predacao:    cacador adjacente a lobo    -> HUNT_CHANCE_PCT% de matar
 *   3. Protecao:    o cacador protege a ovelha INDIRETAMENTE, reduzindo a
 *                   populacao de lobos ao longo das rodadas
 *   4. Vulnerab.:   ovelha e vulneravel ao lobo; lobo e vulneravel ao cacador
 *   5. Reproducao:  mesma faccao adjacente -> REPRO_CHANCE_PCT% de chance
 *
 * Por que snapshot?
 *   A reproducao pode adicionar entidades novas na lista durante
 *   o loop. Sem snapshot, essas entidades seriam processadas como
 *   agressoras na mesma rodada que nasceram - comportamento errado.
 *
 * Por que mark-and-sweep e nao liberar na hora?
 *   Se eu free() uma entidade no meio do loop, o ponteiro ->next que
 *   eu salvei pode ser invalido (dangling pointer). Entao primeiro
 *   marco tudo como morto (mark), termino o loop inteiro e so
 *   depois libero a memoria (sweep).
 *
 * Por que a condicao cur->id < nb->id na reproducao?
 *   Garante que cada par (A, B) reproduz so uma vez por rodada.
 *   Quando A processa B (A.id < B.id): cria filho.
 *   Quando B processa A (B.id > A.id): condicao falsa, ignora.
 *   Sem isso cada par criaria dois filhos.
 */
void interaction_process_all(Board* board, EntityList* list) {
    int count = list->total;
    if (count == 0) return;

    Entity** snap = (Entity**)malloc(count * sizeof(Entity*));
    if (!snap) return;

    Entity* e = list->head;
    int     i = 0;
    while (e && i < count) {
        snap[i++] = e;
        e = e->next;
    }

    /* --- FASE MARK: processa interacoes ----------------------- */
    for (int k = 0; k < i; k++) {
        Entity* cur = snap[k];
        if (!cur->alive) continue;

        for (int d = 0; d < 4; d++) {
            int nr = cur->row + DR[d];
            int nc = cur->col + DC[d];

            if (!board_is_valid(board, nr, nc)) continue;

            Entity* nb = board->grid[nr][nc].entity;
            if (!nb || !nb->alive) continue;

            /* lobo ataca ovelha - mata direto */
            if (cur->faction == FACTION_WOLF &&
                nb->faction  == FACTION_SHEEP) {

                board_remove_entity(board, nb);
                entity_mark_dead(list, nb);
            }
            /* cacador ataca lobo - chance de matar */
            else if (cur->faction == FACTION_HUNTER &&
                     nb->faction  == FACTION_WOLF) {

                if ((rand() % 100) < HUNT_CHANCE_PCT) {
                    board_remove_entity(board, nb);
                    entity_mark_dead(list, nb);
                }
            }
            /* mesma faccao: tenta reproduzir */
            else if (cur->faction == nb->faction && cur->id < nb->id) {
                if ((rand() % 100) < REPRO_CHANCE_PCT) {
                    interaction_reproduce(board, list, cur, nb);
                }
            }
        }
    }

    free(snap);

    /* --- FASE SWEEP: libera entidades mortas ------------------ */
    entity_sweep_dead(list);
}

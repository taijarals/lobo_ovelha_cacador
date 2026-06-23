#ifndef INTERACTION_H
#define INTERACTION_H

#include "board.h"

/* tenta criar um filho em celula vazia adjacente */
int interaction_reproduce(Board* board, EntityList* list,
                          const Entity* e1, const Entity* e2);

/* processa todas as interacoes da rodada (cadeia alimentar + reproducao) */
void interaction_process_all(Board* board, EntityList* list);

#endif

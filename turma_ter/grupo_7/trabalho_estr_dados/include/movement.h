#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "board.h"

/* move uma entidade pra celula vazia adjacente (aleatoriamente) */
int  movement_move_entity(Board* board, Entity* e);

/* move todas as entidades vivas da lista */
void movement_move_all   (Board* board, EntityList* list);

#endif

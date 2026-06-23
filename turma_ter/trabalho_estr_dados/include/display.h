#ifndef DISPLAY_H
#define DISPLAY_H

#include "board.h"
#include "entity.h"

void display_board      (const Board* board);
void display_stats      (const EntityList* list, int round);
void display_legend     (void);
void display_victory    (const EntityList* list, int round);
void display_final_stats(const EntityList* list, int round);

#endif

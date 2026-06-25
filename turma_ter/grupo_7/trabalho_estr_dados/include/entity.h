#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"

/*
 * Entidade = um animal vivo no mapa
 * Decidi usar lista duplamente encadeada porque a remocao no meio
 * fica O(1) com os ponteiros prev/next, sem precisar percorrer tudo
 */
typedef struct Entity {
    int            id;
    FactionType    faction;
    int            row;
    int            col;
    int            health;
    int            alive;        /* 1 = vivo, 0 = morreu (ainda na lista) */
    int            rounds_alive; /* quantas rodadas sobreviveu */
    struct Entity* prev;
    struct Entity* next;
} Entity;

/*
 * Cabeca da lista - guarda contadores separados por faccao
 * assim nao preciso percorrer tudo pra saber quem ganhou
 */
typedef struct {
    Entity* head;
    Entity* tail;
    int     total;
    int     wolves;
    int     sheep;
    int     hunters;
    int     next_id;
} EntityList;

/* funcoes de ciclo de vida */
Entity* entity_create      (EntityList* list, FactionType faction, int row, int col);
void    entity_mark_dead   (EntityList* list, Entity* e);
void    entity_sweep_dead  (EntityList* list);
void    entity_list_init   (EntityList* list);
void    entity_list_destroy(EntityList* list);

/* utilitarios */
char        entity_symbol(FactionType faction);
const char* faction_name (FactionType faction);

#endif
